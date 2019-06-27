#include "ovpCBoxAlgorithmScoringTool.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

namespace OpenViBEPlugins {

	namespace SignalProcessing
	{


		void CBoxAlgorithmScoringTool::setStimulation(const uint32 ui32StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate)
		{
			boolean l_bStateUpdated = false;

			switch (ui64StimulationIdentifier) // Different phases
			{
			case OVTK_GDF_End_Of_Trial:
				m_eCurrentState = EScoreInterfaceState_Idle;
				l_bStateUpdated = true;

				break;

			case OVTK_GDF_End_Of_Session:
				m_eCurrentState = EScoreInterfaceState_Idle;
				l_bStateUpdated = true;

				break;

			case OVTK_GDF_Cross_On_Screen:
				m_eCurrentState = EScoreInterfaceState_Reference;
				l_bStateUpdated = true;

				break;

			case OVTK_GDF_Beep:
				// gdk_beep();
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Trace << "Beep is no more considered in 'Graz Visu', use the 'Sound player' for this!\n";
#if 0
#if defined TARGET_OS_Linux
				system("cat /local/ov_beep.wav > /dev/dsp &");
#endif
#endif
				break;




				//Interesting cases : either a right or left Stimulation
			case OVTK_GDF_Left:
				m_eCurrentState = EScoreInterfaceState_Cue; //Coresponds to the phase
				m_eCurrentDirection = EDirectionScore_Left; //Corresponds to the direction
				l_bStateUpdated = true;
				//test2 = true;

				break;

			case OVTK_GDF_Right:
				m_eCurrentState = EScoreInterfaceState_Cue;
				//test = true;
				m_eCurrentDirection = EDirectionScore_Right;

				l_bStateUpdated = true;

				break;
				//This two cases can be used for over developpment but aren't used now

			case OVTK_GDF_Up:
				m_eCurrentState = EScoreInterfaceState_Cue;
				//test = true;
				m_eCurrentDirection = EDirectionScore_Up;

				l_bStateUpdated = true;
				break;

			case OVTK_GDF_Down:
				m_eCurrentState = EScoreInterfaceState_Cue;
				//test = true;
				m_eCurrentDirection = EDirectionScore_Down;

				l_bStateUpdated = true;
				break;

			case OVTK_GDF_Feedback_Continuous:
				// New trial starts

				m_eCurrentState = EScoreInterfaceState_ContinousFeedback;


				// as some trials may have artifacts and hence very high responses from e.g. LDA
				// its better to reset the max between trials

				l_bStateUpdated = true;

				break;
			}


		}

		void CBoxAlgorithmScoringTool::MatrixDistanceLDA(const IMatrix* l_pMatrix) //Function that extract the raw data corresponding to the distance to the y = x bar on the classifier
		{

			bool essai = false;
			uint32 Dimension = l_pMatrix->getDimensionCount();
			float Ampli = 0.0;
			const float64 * Data = l_pMatrix->getBuffer();
			float64 Ampl1 = 0;
			float64 Ampl2 = 0;

			if (m_bTwoValueInput)
			{
				if (Data[0] != 0) {
					Ampl1 = Data[0];
				}

				if (Data[1] != 0) {
					Ampl2 = Data[1];
				}

				Ampli = (float)(Ampl2 - Ampl1); //The difference of two epochs is calculated, if it is positive : it means right and negative means left.


				//printf("Matrice sortie : %f\n", je);
			}

			else
			{
				Ampli = (float)Data[0];

			}


			m_vAmplitude.push_back(Ampli); //THe data of the matrix are not stored at the beginning which means that you need to creat an array containing every amplitude
			//printf("Matrice sortie : %f\n",Ampli);
			m_fvelocity = aggregatePredictions(false);//The "velocity"/"intensity" of the signal is then processed via the function aggregate predictions


			

		}


		CBoxAlgorithmScoringTool::CBoxAlgorithmScoringTool(void) :
			m_eCurrentState(EScoreInterfaceState_Idle),
			m_eCurrentDirection(EDirectionScore_None),
			m_i64left(1),
			score(0),
			cpt(1),
			m_bTwoValueInput(false),
			m_fvelocity(0.0),
			memory(0.0),

			m_i64PredictionsToIntegrate(5)
			{

			}



		bool CBoxAlgorithmScoringTool::initialize(void)
		{
			m_oInput0Decoder.initialize(*this, 0);
			m_oInput1Decoder.initialize(*this, 1);
			m_oOutput0Encoder.initialize(*this, 0);
			m_bShowInstruction = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			m_i64PredictionsToIntegrate = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
			m_i64left = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
			m_uiIdleFuncTag = 0;
			cpt = 1;
			score = 0;

			return true;
		}
		/*******************************************************************************/

		bool CBoxAlgorithmScoringTool::uninitialize(void)
		{
			m_oInput0Decoder.uninitialize();
			m_oInput1Decoder.uninitialize();
			m_oOutput0Encoder.uninitialize();
			



			return true;
		}

		bool CBoxAlgorithmScoringTool::processInput(uint32 ui32InputIndex)
		{
			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			return true;
		}

		bool CBoxAlgorithmScoringTool::process(void)
		{
			const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
			// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
			IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

			IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

			//Loop to get the stimulations

			for (uint32 chunk = 0; chunk<l_pBoxIO->getInputChunkCount(0); chunk++)
			{
				m_oInput0Decoder.decode(chunk);
				if (m_oInput0Decoder.isBufferReceived())
				{
					const IStimulationSet* l_pStimulationSet = m_oInput0Decoder.getOutputStimulationSet();
					for (uint32 s = 0; s<l_pStimulationSet->getStimulationCount(); s++)
					{

						setStimulation(s,
							l_pStimulationSet->getStimulationIdentifier(s),
							l_pStimulationSet->getStimulationDate(s));
					}
				}
			}

			//loop to get the raw EEG classified data
			for (uint32 chunk = 0; chunk<l_pBoxIO->getInputChunkCount(1); chunk++)
			{
				m_oInput1Decoder.decode(chunk);
				if (m_oInput1Decoder.isHeaderReceived())
				{
					const IMatrix* l_pMatrix = m_oInput1Decoder.getOutputMatrix();

					if (l_pMatrix->getDimensionCount() == 0)
					{
						this->getLogManager() << LogLevel_Error << "Error, dimension count is 0 for Amplitude input !\n";
						return false;
					}

					if (l_pMatrix->getDimensionCount() > 1)
					{
						for (uint32 k = 1; k<l_pMatrix->getDimensionSize(k); k++)
						{
							if (l_pMatrix->getDimensionSize(k) > 1)
							{
								this->getLogManager() << LogLevel_Error << "Error, only column vectors supported as Amplitude!\n";
								return false;
							}
						}
					}

					if (l_pMatrix->getDimensionSize(0) == 0)
					{
						this->getLogManager() << LogLevel_Error << "Error, need at least 1 dimension in Amplitude input !\n";
						return false;
					}
					else if (l_pMatrix->getDimensionSize(0) >= 2)
					{
						this->getLogManager() << LogLevel_Trace << "Got 2 or more dimensions for feedback, feedback will be the difference between the first two.\n";
						m_bTwoValueInput = true;
					}
				}

				if (m_oInput1Decoder.isBufferReceived())
				{
					MatrixDistanceLDA(m_oInput1Decoder.getOutputMatrix());

				}
				uint64 l_ui64CurrentTime = this->getPlayerContext().getCurrentTime();
				score_processing(m_bShowInstruction ? m_eCurrentDirection : EDirectionScore_None);
				m_oOutput0Encoder.encodeHeader();
				IStimulationSet* l_pStimulationSet = m_oOutput0Encoder.getInputStimulationSet();
				l_pStimulationSet->clear();
				getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);
				l_pStimulationSet->appendStimulation(memory, l_ui64CurrentTime, 0);
				m_oOutput0Encoder.encodeBuffer();

				l_pBoxIO->markOutputAsReadyToSend(0, m_ui64PreviousActivationTime, l_ui64CurrentTime);
				getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

				m_ui64PreviousActivationTime = l_ui64CurrentTime;

			}




			return true;
		}

		float CBoxAlgorithmScoringTool::aggregatePredictions(bool bIncludeAll)
		{
			
			float VoteAggregateUp = 0;
			float VoteAggregateDown = 0;
			float retour = 0;
			int cpt1 = 0;
			int cpt2 = 0;
			float weight1 = 0.5;
			float weight2 = 0.5;
			// Do we have enough predictions to integrate a result?
			if (m_vAmplitude.size() >= m_i64PredictionsToIntegrate)
			{
				// step backwards with rev iter to take the latest samples
				uint64 count = 0;

				//loop that will calculate the mean of the log of the different actions associated with a weight to get more interesting results
				//The weights are calculated with a linear regression
				for (std::deque<float64>::reverse_iterator a = m_vAmplitude.rbegin();
					a != m_vAmplitude.rend() && (bIncludeAll || count<m_i64PredictionsToIntegrate); a++, count++)
				{




					if (*a * 10 >= 1)
					{
						VoteAggregateUp += weight1*log(10 * (float)*a);
						weight1 /= 2;
						cpt1++;
					}
					if (*a * 10 <= -1)
					{
						VoteAggregateDown += -weight2 * log(abs(10 * (float)*a));
						weight2 /= 2;
						cpt2++;
					}

					if ((*a * 10 > -1) && (*a * 10 < 1))
					{
						cpt1 = 1;
						cpt2 = 1;
						VoteAggregateUp = 0.5;
						VoteAggregateDown = -0.5;

					}


				}

				if (cpt1 != 0)
					VoteAggregateUp /= cpt1;
				if (cpt2 != 0)
					VoteAggregateDown /= cpt2;

			}

			

			//At the end amplification and summary of the two actors
			return (VoteAggregateUp + VoteAggregateDown);
		}
		

		void CBoxAlgorithmScoringTool::score_processing(EDirectionScore eDirection)
		{
			switch (m_eCurrentState)
			{
			case EScoreInterfaceState_Idle:
				memory = score;
				score = 0;
				cpt = 1;

				break;
			case EScoreInterfaceState_Cue:
				memory = score;
				score = 0;
				cpt = 1;

				break;
			case EScoreInterfaceState_Reference:
				memory = 0;
				score = 0;
				cpt = 1;

				break;
			case EScoreInterfaceState_ContinousFeedback:
				score = score + m_fvelocity;
				cpt += 1;
				score = score / cpt;
				printf("Score = %f\n", score);
				switch (eDirection)
				{
				case EDirectionScore_None:
					//horizontal line
					break;

				case EDirectionScore_Left:
					if (m_i64left == 1)
					{
						if (score <= 0)
						{
							printf("C'est gagné");
						}
					}
					break;

				case EDirectionScore_Right:
					if (m_i64left == 2)
					{
						if (score > 0)
						{
							printf("C'est gagné");
						}
					}

					break;
				}


				memory = 0;
				break;
			}
			
			
			
			
			
		}



	};

};