#include <ctime>
#include "VICamera.h"

#include "opencv2/opencv.hpp"

/*****************************
* スレッド
*/

// 撮影運転、照明のON/OFF
void threadGrabbing(VICamera* camera)
{
	int lightInterval = camera->LightInterval();
	int lightOnTime = camera->LightOnTime();
	int delayShutterTime = camera->DelayShutterTime();

	lightInterval -= lightOnTime;
	lightOnTime -= delayShutterTime;

	assert(lightInterval > 0);
	assert(lightOnTime > 0);

	// Can the camera device be queried whether it is ready to accept the next frame trigger?
	if (camera->CanWaitForFrameTriggerReady())
	{
		// Start the grabbing using the grab loop thread, by setting the grabLoopType parameter
		// to GrabLoop_ProvidedByInstantCamera. The grab results are delivered to the image event handlers.
		// The GrabStrategy_OneByOne default grab strategy is used.
		//camera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		// Wait for user input to trigger the camera or exit the program.
		// The grabbing is stopped, the device is closed and destroyed automatically when the camera object goes out of scope.
		do
		{
			// 照明間隔待ち
			Sleep(lightInterval);

			// 照明ON
			camera->SendOn();

			// シャッター待ち
			Sleep(delayShutterTime);

			// シャッタートリガ
			// Execute the software trigger. Wait up to 1000 ms for the camera to be ready for trigger.
			if (camera->WaitForFrameTriggerReady(100, Pylon::TimeoutHandling_ThrowException))
			{
				if (camera->GrabMode()) {
					camera->ExecuteSoftwareTrigger();
				}
			}

			// 照明ON、残り
			Sleep(lightOnTime);

			// 照明OFF
			camera->SendOff();

		} while (camera->IsGrabbing());
	}

	// 照明クローズ
	camera->TcpClose();
}

// 保存スレッド
void threadSave(const Pylon::CGrabResultPtr& ptrGrabResult) {
	// タイムスタンプでファイル名作成
	char buf[32];
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	sprintf_s(buf, 32, "%04d%02d%02d_%02d%02d%02d_%03d.png", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);

	//Pylon::CImagePersistence::Save(Pylon::EImageFileFormat::ImageFileFormat_Raw, buf, ptrGrabResult);
	cv::Mat image(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, ptrGrabResult->GetBuffer());
	cv::imwrite(buf, image);

}

/************************************/

/**
* コンフィグ
*/
#include "SoftTriggerCustomize.h"
#if 0
class  CSoftTriggerCustomize : public Pylon::CSoftwareTriggerConfiguration {

public:
	static void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {
		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration" << std::endl;

		//CEnumParameter(nodemap, "AcquisitionFrameRate").SetValue("200");




	}

	//Set basic camera settings.
	virtual void OnOpened(Pylon::CInstantCamera& camera)
	{
		try
		{
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
#endif

class CSampleConfigurationEventPrinter : public Pylon::CConfigurationEventHandler
{
public:
	void OnAttach(Pylon::CInstantCamera& /*camera*/)
	{
		std::cout << "OnAttach event" << std::endl;
	}

	void OnAttached(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnOpen(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnOpened(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStart(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStarted(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStop(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStopped(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnClose(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnClose event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnClosed(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDestroy(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDestroyed(Pylon::CInstantCamera& /*camera*/)
	{
		std::cout << "OnDestroyed event" << std::endl;
	}

	void OnDetach(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDetached(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage)
	{
		std::cout << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		std::cout << "Error Message: " << errorMessage << std::endl;
	}

	void OnCameraDeviceRemoved(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnCameraDeviceRemoved event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}
};

/**
* イベントハンドラ
*/
class CSampleImageEventPrinter : public Pylon::CImageEventHandler
{
private:
	VICamera* pCamera;
public:
	CSampleImageEventPrinter(VICamera* camera) {
		pCamera = camera;
	}

	virtual void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
	{
		std::cout << "OnImagesSkipped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		std::cout << countOfSkippedImages << " images have been skipped." << std::endl;
		std::cout << std::endl;
	}


	virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult)
	{
		std::cout << "OnImageGrabbed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;

		// Image grabbed successfully?
		if (ptrGrabResult->GrabSucceeded())
		{
			char buf[32];
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			sprintf_s(buf, 32, "%04d%02d%02d_%02d%02d%02d_%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);

			std::cout << "Time: " << buf << std::endl;

			std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
			std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;
			const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
			std::cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << std::endl;
			std::cout << std::endl;

			Pylon::DisplayImage(1, ptrGrabResult);

			// 保存するか
			if (!pCamera->IsSaved()) {
				std::thread th(threadSave, ptrGrabResult);
				th.detach();
				pCamera->ItSaved();
			}

		}
		else
		{
			std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
		}
	}
};



VICamera::VICamera()
{
	Open();
}

VICamera::~VICamera()
{
	Close();
}

void VICamera::Open()
{
	init();
}

void VICamera::Close()
{
	if (!opened) return;

	if (camera.IsOpen()) camera.Close();

	if (typeGain == GAIN_TYPE::GAIN_DOUBLE)
	{
		doubleGain.Release();
	}
	else
	{
		intGain.Release();
	}

	camera.DestroyDevice();

	opened = false;
}

void VICamera::StartGrabbing(Pylon::EGrabStrategy strategy, Pylon::EGrabLoop grabLoopType)
{
	if (flagGrabbing) return;

	flagGrabbing = true;

	try {
		// 照明
		TcpConnect();

		// カメラ
		camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		RunGrabbing();
	}
	catch (const Pylon::GenericException& e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}

}

void VICamera::StopGrabbing()
{
	flagGrabbing = false;

	Sleep(lightInterval);

	try {
		// Camera may have been disconnected.
		if (camera.IsGrabbing())
		{
			camera.StopGrabbing();
		}

		// 照明クローズ
		//TcpClose();

	}
	catch (const Pylon::GenericException& e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}
}

void VICamera::RunGrabbing()
{
	std::thread th(threadGrabbing, this);
	th.detach();
}

/**
* 初期化
* 
* pylon の初期化
* デバイスの割付
* パラメータ設定
* イベントハンドラ設定
* Open
* ゲインノード取得
*/
void VICamera::init()
{
	if (opened) return;

	// Before using any pylon methods, the pylon runtime must be initialized. 
	Pylon::PylonInitialize();

	try {
		// Create an instant camera object with the first camera device found.
		camera.Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice());

		// パラメータ設定
		camera.RegisterConfiguration(new CSoftTriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

		// イベントハンドラ設定
		camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
		camera.RegisterImageEventHandler(new CSampleImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

		// デバイスオープン
		// Open the camera for accessing the parameters.
		camera.Open();

		// ゲインノード取得
		if (camera.GetSfncVersion() >= Pylon::Sfnc_2_0_0)
		{
			// double
			typeGain = GAIN_TYPE::GAIN_DOUBLE;

			doubleGain.Attach(camera.GetNodeMap().GetNode("Gain"));

			doubleGainMax = doubleGain.GetMax();
			doubleGainMin = doubleGain.GetMin();
		}
		else
		{
			// long long
			typeGain = GAIN_TYPE::GAIN_INT64;

			intGain.Attach(camera.GetNodeMap().GetNode("GainRaw"));

			intGainMax = intGain.GetMax();
			intGainMin = intGain.GetMin();
		}
	}
	catch (const Pylon::GenericException& e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}

	opened = true;
}

void VICamera::DisplayGainSlider()
{
	if (IsGainDouble()) {
		float gain = (float)doubleGain.GetValue();
		float min = (float)doubleGainMin;
		float max = (float)doubleGainMax;
		ImGui::SliderFloat("gain", &gain, min, max, "%.1f");
		ImGui::InputFloat("", &gain, 0.1f, 0.5f, "%.1f");
		
		doubleGain.SetValue((double)gain);
	}
	else {
		int gain = (int)intGain.GetValue();
		int min = (int)intGainMin;
		int max = (int)intGainMax;
		ImGui::SliderInt("gain", &gain, min, max);
		ImGui::InputInt("", &gain);

		intGain.SetValue((int64_t)gain);
	}
}


