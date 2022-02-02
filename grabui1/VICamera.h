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
	// �b��
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
	// �B�e�J�n
	void StartGrabbing(Pylon::EGrabStrategy strategy, Pylon::EGrabLoop grabLoopType);

	// �B�e�I��
	void StopGrabbing();

	// �A���B�e
	void RunGrabbing();

	// �Q�C���ύX
	/* GainSelector �őI�����邱�Ƃ��K�v */

	bool IsGainDouble() {
		return typeGain == GAIN_TYPE::GAIN_DOUBLE;
	}

private:
	void init();

private:
	// �J�����f�o�C�X
	Pylon::CInstantCamera camera;
	//Pylon::CBaslerUniversalInstantCamera camera;

	// �Q�C���̃m�[�h�^�C�v
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};

	enum class GAIN_TYPE typeGain;

	// �Q�C���̃m�[�h�Adouble
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;

	// �Q�C���̃m�[�h�Along long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;

	// �B�e�p���t���O
	bool flagGrabbing;

	// �I�o���Ԃ̃m�[�h
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;

	// 255�l�ɂȂ����s�N�Z���̌�
	int pixel255;

public:
	bool IsGrabbing() {
		return flagGrabbing;
	}

	// �Q�C���̃X���C�h�o�[������
	void DisplayGainSlider();

	// �Q�C���̂��Ƃ�
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

	// 255�� pixel �̐�
	int GetPixel255() { return pixel255; }
	void SetPixel255(int val) { pixel255 = val; }

private:
	// �ۑ����ꂽ
	bool flagSaved = true;

	// �B�e����
	bool flagGrabMode = false;

	// �Ɩ�������
	bool flagLight = false;

public:
	// �B�e����
	void GoGrab(bool goGrab) {
		flagGrabMode = goGrab;
	}

	// �B�e���邩�ǂ���
	bool GrabMode() {
		return flagGrabMode;
	}

	// �ۑ�����
	void SaveNow() {
		flagSaved = false;
	}

	// �ۑ�����
	void ItSaved() {
		flagSaved = true;
	}

	// �ۑ��ł������ǂ���
	bool IsSaved() {
		return flagSaved;
	}

	// �Ɩ�������
	void OnLight() {
		flagLight = true;
	}

	// �Ɩ�������
	void OffLight() {
		flagLight = false;
	}

	// �Ɩ��̏��
	bool IsLight() {
		return flagLight;
	}

private:
	// �Ɩ�
	int lightInterval = 1000;
	int lightOnTime = 100;
	int delayShutterTime = 50;

public:
	// �Ɩ�
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

