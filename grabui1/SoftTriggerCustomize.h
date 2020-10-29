#pragma once
#include "TemplateParameterSet.h"

#include "opencv2/opencv.hpp"
/**
* �R���t�B�O
*/
class  CSoftTriggerCustomize : public Pylon::CSoftwareTriggerConfiguration {

private:

	// �ő�o�b�t�@��
	const int iMaxNumBuffer = 10;

	// �F�␳
	// ���C�}�b�N ��LED 80%  2020.9.30
	const double BGRLight_Rate = 0.75;
	const double BGRLight_BRate = 0.8;
	const double BGRLight_GRate = 1.0;
	const double BGRLight_RRate = 0.75;
	const double BLight_B = 223 * BGRLight_BRate * BGRLight_Rate;
	const double BLight_G = 70 * BGRLight_BRate * BGRLight_Rate;
	const double BLight_R = 10 * BGRLight_BRate * BGRLight_Rate;
	const double GLight_B = 68 * BGRLight_GRate * BGRLight_Rate;
	const double GLight_G = 247 * BGRLight_GRate * BGRLight_Rate;
	const double GLight_R = 20 * BGRLight_GRate * BGRLight_Rate;
	const double RLight_B = 5 * BGRLight_RRate * BGRLight_Rate;
	const double RLight_G = 49 * BGRLight_RRate * BGRLight_Rate;
	const double RLight_R = 170 * BGRLight_RRate * BGRLight_Rate;
#if 0
	// ���C�}�b�NLED 100% ����25mm
	const double BLight_B = 223.;
	const double BLight_G = 70.;
	const double BLight_R = 10.;
	const double GLight_B = 68.;
	const double GLight_G = 247.;
	const double GLight_R = 20.;
	const double RLight_B = 5.;
	const double RLight_G = 49.;
	const double RLight_R = 170.;

#endif


public:
	//static 
	void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {

		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration" << std::endl;

		using namespace Pylon;

		// �I�����Ԃ̎w��i0�̏ꍇ�͕ύX���Ȃ�
		SetValue<CEnumParameter, const char*>(nodemap, "ExposureMode", "Timed");
		SetValue<CFloatParameter, double>(nodemap, "ExposureTime", 1000.0);

		// �s�N�Z���t�H�[�}�b�g
		SetValue<CEnumParameter, const char*>(nodemap, "PixelFormat", "BayerRG8");

		// acquisition start trigger�@�\�𖳌����́A���Ȃ�

		// �f���U�C�N���[�h��BaslerPGI�ABayer�ȊO�łȂ��Ɛݒ�ł��Ȃ�
		TrySetValue<CEnumParameter, const char*>(nodemap, "DemosaicingMode", "BaslerPGI");

		// LightSourcePreset
		if (SetValue<CEnumParameter, const char*>(nodemap, "LightSourcePreset", "Off"))
		{
			//CEnumParameter(nodemap, "BalanceWhiteAuto").SetValue("Off");

			// OFF �ɂł����̂�
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Blue",  "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Green", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Red",   "BalanceRatio", 1.0);
			
			// �z���C�g�o�����X��C�Ӓ���
            // �����p�}�g���N�X�v�Z
			double lightMatrix[3][3] = {
				{ RLight_R / 255., GLight_R / 255., BLight_R / 255. },
				{ RLight_G / 255., GLight_G / 255., BLight_G / 255. },
				{ RLight_B / 255., GLight_B / 255., BLight_B / 255. }
			};

			// �t�s��
			cv::Mat invLightMat = cv::Mat(3, 3, CV_64F, lightMatrix).inv();

			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain00", "ColorTransformationValue", invLightMat.at<double>(0, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain01", "ColorTransformationValue", invLightMat.at<double>(0, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain02", "ColorTransformationValue", invLightMat.at<double>(0, 2));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain10", "ColorTransformationValue", invLightMat.at<double>(1, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain11", "ColorTransformationValue", invLightMat.at<double>(1, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain12", "ColorTransformationValue", invLightMat.at<double>(1, 2));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain20", "ColorTransformationValue", invLightMat.at<double>(2, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain21", "ColorTransformationValue", invLightMat.at<double>(2, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain22", "ColorTransformationValue", invLightMat.at<double>(2, 2));

		}

		// �Q�C��
		SetValue<CEnumParameter, const char*>(nodemap, "GainAuto", "Off");
		SetValue<CEnumParameter, const char*>(nodemap, "GainSelector", "All");

#if 0
		// �Ɩ�����
		CEnumParameter(nodemap, "LineSelector").SetValue("Line2");
		CEnumParameter(nodemap, "LineSource").SetValue("ExposureActive");
#endif

	}

	void ApplyInstantCameraParameter(Pylon::CInstantCamera& camera) {
		std::cout << "OnOpened -> ApplyInstantParameter" << std::endl;

		// �ő�o�b�t�@��
		camera.MaxNumBuffer = iMaxNumBuffer;

		
	}

	//Set basic camera settings.
	virtual void OnOpened(Pylon::CInstantCamera& camera)
	{
		try
		{
			ApplyInstantCameraParameter(camera);
			ApplyConfiguration(camera.GetNodeMap());
		}
		catch (const Pylon::GenericException& e)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. Pylon::GenericException caught in OnOpened method msg=%hs", e.what());
		}
		catch (const std::exception& e)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. std::exception caught in OnOpened method msg=%hs", e.what());
		}
		catch (...)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. Unknown exception caught in OnOpened method.");
		}
	}
};