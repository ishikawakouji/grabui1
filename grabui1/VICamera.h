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
	// b’è
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
	// B‰eŠJn
	void StartGrabbing(Pylon::EGrabStrategy strategy, Pylon::EGrabLoop grabLoopType);

	// B‰eI—¹
	void StopGrabbing();

	// ˜A‘±B‰e
	void RunGrabbing();

	// ƒQƒCƒ“•ÏX
	/* GainSelector ‚Å‘I‘ğ‚·‚é‚±‚Æ‚ª•K—v */

	bool IsGainDouble() {
		return typeGain == GAIN_TYPE::GAIN_DOUBLE;
	}

private:
	void init();

private:
	// ƒJƒƒ‰ƒfƒoƒCƒX
	Pylon::CInstantCamera camera;
	//Pylon::CBaslerUniversalInstantCamera camera;

	// ƒQƒCƒ“‚Ìƒm[ƒhƒ^ƒCƒv
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};

	enum class GAIN_TYPE typeGain;

	// ƒQƒCƒ“‚Ìƒm[ƒhAdouble
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;

	// ƒQƒCƒ“‚Ìƒm[ƒhAlong long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;

	// B‰eŒp‘±ƒtƒ‰ƒO
	bool flagGrabbing;

public:
	bool IsGrabbing() {
		return flagGrabbing;
	}

	// ƒQƒCƒ“‚ÌƒXƒ‰ƒCƒhƒo[‚ğ‘‚­
	void DisplayGainSlider();

	// ƒQƒCƒ“‚Ì‚â‚è‚Æ‚è
	int64_t GetIntGain() { return intGain.GetValue(); }
	double GatDoubleGain() { return doubleGain.GetValue(); }

	void SetIntGain(int64_t val) { intGain.SetValue(val); }
	void SetDoubleGain(double val) { doubleGain.SetValue(val); }

	int64_t GetIntGainMax() { return intGainMax; }
	int64_t GetIntGainMin() { return intGainMin; }

	double GetDoubleGainMax() { return doubleGainMax; }
	double GetDoubleGainMin() { return doubleGainMin; }

private:
	// •Û‘¶‚³‚ê‚½
	bool flagSaved = true;

	// B‰e‚·‚é
	bool flagGrabMode = false;

public:
	// B‰e‚·‚é
	void GoGrab(bool goGrab) {
		flagGrabMode = goGrab;
	}

	// B‰e‚·‚é‚©‚Ç‚¤‚©
	bool GrabMode() {
		return flagGrabMode;
	}

	// •Û‘¶‚·‚é
	void SaveNow() {
		flagSaved = false;
	}

	// •Û‘¶‚µ‚½
	void ItSaved() {
		flagSaved = true;
	}

	// •Û‘¶‚Å‚«‚½‚©‚Ç‚¤‚©
	bool IsSaved() {
		return flagSaved;
	}

private:
	// Æ–¾
	int lightInterval = 1000;
	int lightOnTime = 100;
	int delayShutterTime = 50;

public:
	// Æ–¾
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
#	define COMMAND_SIZE 16
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

