#ifndef __OpenViBEPlugins_BoxAlgorithm_ScoringTool_H__
#define __OpenViBEPlugins_BoxAlgorithm_ScoringTool_H__

//You may have to change this path to match your folder organisation
#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>


#include <vector>
#include <string>
#include <map>
#include <deque>
// The unique identifiers for the box and its descriptor.
// Identifier are randomly chosen by the skeleton-generator.
#define OVP_ClassId_BoxAlgorithm_ScoringTool (0x34cd5f22, 0xb9d6e6d3)
#define OVP_ClassId_BoxAlgorithm_ScoringToolDesc (0xa36513a2, 0x19ef3685)
#define OV_AttributeId_Box_FlagIsUnstable OpenViBE::CIdentifier(0x666FFFFF, 0x666FFFFF)


namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{

		enum EDirectionScore
		{
			EDirectionScore_None = 0,
			EDirectionScore_Left,
			EDirectionScore_Right,
			EDirectionScore_Up,
			EDirectionScore_Down,
		};

		enum EScoreInterfaceState
		{
			EScoreInterfaceState_Idle,
			EScoreInterfaceState_Reference,
			EScoreInterfaceState_Cue,
			EScoreInterfaceState_ContinousFeedback
		};
		/**
		 * \class CBoxAlgorithmScoringTool
		 * \author Tristan (Sorbonne Université)
		 * \date Thu Jun 27 11:12:27 2019
		 * \brief The class CBoxAlgorithmScoringTool describes the box ScoringTool.
		 *
		 */
		class CBoxAlgorithmScoringTool : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			CBoxAlgorithmScoringTool(void);
			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(OpenViBE::uint32 ui32InputIndex);
			//Here is the different process callbacks possible
			// - On clock ticks :
			//virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);		
			// - On new input received (the most common behaviour for signal processing) :
			//virtual bool processInput(uint32_t ui32InputIndex);
			
			// If you want to use processClock, you must provide the clock frequency.
			//virtual uint64_t getClockFrequency(void);
			
			virtual bool process(void);

			virtual void score_processing(EDirectionScore eDirection);
			
			// As we do with any class in openvibe, we use the macro below 
			// to associate this box to an unique identifier. 
			// The inheritance information is also made available, 
			// as we provide the superclass OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
			

		protected:
			// Input decoder:
			OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmScoringTool > m_oInput0Decoder;
			OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmScoringTool > m_oInput1Decoder;

			EScoreInterfaceState m_eCurrentState;
			EDirectionScore m_eCurrentDirection;
			// Output decoder:
			OpenViBEToolkit::TStimulationEncoder < CBoxAlgorithmScoringTool> m_oOutput0Encoder;

			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);
			virtual void MatrixDistanceLDA(const OpenViBE::IMatrix* l_pMatrix);
			virtual float aggregatePredictions(bool bIncludeAll);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ScoringTool);

		public:
			OpenViBE::uint64 m_i64left;
			std::deque<OpenViBE::float64> m_vAmplitude;
			float m_fvelocity;
			bool m_bShowInstruction;
			bool m_bTwoValueInput;
			float score;
			int cpt;

			OpenViBE::uint64 choice;
			OpenViBE::uint64 gainE;
			OpenViBE::uint64 gainS;

			float maxAmpl;

			OpenViBE::uint64 m_ui64PreviousActivationTime;
			OpenViBE::uint64 m_i64PredictionsToIntegrate;
			int m_uiIdleFuncTag;
			float memory;

		};


		// If you need to implement a box Listener, here is a sekeleton for you.
		// Use only the callbacks you need.
		// For example, if your box has a variable number of input, but all of them must be stimulation inputs.
		// The following listener callback will ensure that any newly added input is stimulations :
		/*		
		virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
		{
			rBox.setInputType(ui32Index, OV_TypeId_Stimulations);
		};
		*/
		
		/*
		// The box listener can be used to call specific callbacks whenever the box structure changes : input added, name changed, etc.
		// Please uncomment below the callbacks you want to use.
		class CBoxAlgorithmScoringToolListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			//virtual bool onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
			//virtual bool onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			//virtual bool onInputConnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputConnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
			//virtual bool onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		*/

		/**
		 * \class CBoxAlgorithmScoringToolDesc
		 * \author Tristan (Sorbonne Université)
		 * \date Thu Jun 27 11:12:27 2019
		 * \brief Descriptor of the box ScoringTool.
		 *
		 */
		class CBoxAlgorithmScoringToolDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("ScoringTool"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Tristan"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Sorbonne Université"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Print out the score of the person"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("The box will send a score as an output that can be read by a visual reward"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-bold"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ScoringTool; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmScoringTool; }
			
			/*
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmScoringToolListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			*/
			virtual bool getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Stimulations",OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Ampltude",OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				
				rBoxAlgorithmPrototype.addOutput("Score",OV_TypeId_Stimulations);

				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);

				rBoxAlgorithmPrototype.addSetting("Show instruction",OV_TypeId_Boolean,"true");
				rBoxAlgorithmPrototype.addSetting("Predictions to integrate",OV_TypeId_Integer,"5");
				rBoxAlgorithmPrototype.addSetting("Left (1) or Right (2)", OV_TypeId_Integer, "1");

				rBoxAlgorithmPrototype.addSetting("Choix du systeme de regulation : logarithme (1) direct (2)", OV_TypeId_Integer, "1");
				rBoxAlgorithmPrototype.addSetting("Logartithme : Gain d'entree", OV_TypeId_Integer, "5");
				rBoxAlgorithmPrototype.addSetting("Logartithme : Gain de sortie", OV_TypeId_Integer, "300");

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				
				rBoxAlgorithmPrototype.addFlag(OV_AttributeId_Box_FlagIsUnstable);
				
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ScoringToolDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ScoringTool_H__
