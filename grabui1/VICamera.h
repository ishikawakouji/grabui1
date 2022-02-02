#pragma once

#include <thread>
#include "imgui.h"

#include "TcpSender.h"

// camera
// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
//#include <pylon/BaslerUniversalInstantCamera.h>
#define PYLON_WIN_BUILD
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

class VICamera
{
private:
	bool opened = false;
public:
	VICamera();
	~VICamera();

	void Open();
	void Close();

#if 1
	// 暫定
	bool CanWaitForFrameTriggerReady() {
		return camera.CanWaitForFrameTriggerReady();
	}

	bool WaitForFrameTriggerReady(unsigned int timeoutMs, Pylon::ETimeoutHandling timeoutHandling) {
		return camera.WaitForFrameTriggerReady(timeoutMs, timeoutHandling);
	}

	void ExecuteSoftwareTrigger() {
		camera.ExecuteSoftwareTrigger();
	}

#endif


public:
	// 撮影開始
	void StartGrabbing(Pylon::EGrabStrategy strategy, Pylon::EGrabLoop grabLoopType);

	// 撮影終了
	void StopGrabbing();

	// 連続撮影
	void RunGrabbing();

	// ゲイン変更
	/* GainSelector で選択することが必要 */

	bool IsGainDouble() {
		return typeGain == GAIN_TYPE::GAIN_DOUBLE;
	}

private:
	void init();

private:
	// カメラデバイス
	Pylon::CInstantCamera camera;
	//Pylon::CBaslerUniversalInstantCamera camera;

	// ゲインのノードタイプ
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};

	enum class GAIN_TYPE typeGain;

	// ゲインのノード、double
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;

	// ゲインのノード、long long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;

	// 撮影継続フラグ
	bool flagGrabbing;

	// 露出時間のノード
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;

	// 255値になったピクセルの個数
	int pixel255;

public:
	bool IsGrabbing() {
		return flagGrabbing;
	}

	// ゲインのスライドバーを書く
	void DisplayGainSlider();

	// ゲインのやりとり
	int64_t GetIntGain() { return intGain.GetValue(); }
	double GatDoubleGain() { return doubleGain.GetValue(); }

	void SetIntGain(int64_t val) { intGain.SetValue(val); }
	void SetDoubleGain(double val) { doubleGain.SetValue(val); }

	int64_t GetIntGainMax() { return intGainMax; }
	int64_t GetIntGainMin() { return intGainMin; }

	double GetDoubleGainMax() { return doubleGainMax; }
	double GetDoubleGainMin() { return doubleGainMin; }

	double GetDoubleExposureTime() {
		if (flagExposureTimeValid) {
			return doubleExposureTime.GetValue();
		}
		return 1000.0;
	}
	void SetDoubleExposureTime(double val) {
		if (flagExposureTimeValid) {
			doubleExposureTime.SetValue(val);
		}
	}

	// 255の pixel の数
	int GetPixel255() { return pixel255; }
	void SetPixel255(int val) { pixel255 = val; }

private:
	// 保存された
	bool flagSaved = true;

	// 撮影する
	bool flagGrabMode = false;

	// 照明をつける
	bool flagLight = false;

public:
	// 撮影する
	void GoGrab(bool goGrab) {
		flagGrabMode = goGrab;
	}

	// 撮影するかどうか
	bool GrabMode() {
		return flagGrabMode;
	}

	// 保存する
	void SaveNow() {
		flagSaved = false;
	}

	// 保存した
	void ItSaved() {
		flagSaved = true;
	}

	// 保存できたかどうか
	bool IsSaved() {
		return flagSaved;
	}

	// 照明をつける
	void OnLight() {
		flagLight = true;
	}

	// 照明を消す
	void OffLight() {
		flagLight = false;
	}

	// 照明の状態
	bool IsLight() {
		return flagLight;
	}

private:
	// 照明
	int lightInterval = 1000;
	int lightOnTime = 100;
	int delayShutterTime = 50;

public:
	// 照明
	void SetLightInterval(int val) {
		lightInterval = val;
	}

	int LightInterval() { return lightInterval; };

	void SetLightOnTime(int val) {
		lightOnTime = val;
	}

	int LightOnTime() { return lightOnTime; };

	void SetDelayShutterTime(int val) {
		delayShutterTime = val;
	}

	int DelayShutterTime() { return delayShutterTime; };

private:
	// TCP sender
	TcpSender power;

	// power command
	char onCommand[COMMAND_SIZE];
	char offCommand[COMMAND_SIZE];

public:
	void SetCommand(char* on, char* off) {
		memcpy_s(onCommand, COMMAND_SIZE, on, COMMAND_SIZE);
		memcpy_s(offCommand, COMMAND_SIZE, off, COMMAND_SIZE);
	}

	void SetIpAddress(char* ipaddr, int port) {
		power.SetServer(ipaddr, port);
	}

	void TcpConnect() { 
		int ret = power.Connect();
	}

	void SendOn() {
		power.Send(onCommand);
	}

	void SendOff() {
		power.Send(offCommand);
	}


	void TcpClose() {
		int ret = power.Close();
	}
};

