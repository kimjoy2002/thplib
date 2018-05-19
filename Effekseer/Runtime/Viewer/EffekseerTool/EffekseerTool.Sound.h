
#ifndef	__EFFEKSEERTOOL_SOUND_H__
#define	__EFFEKSEERTOOL_SOUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSound.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Sound
{
private:
	::EffekseerSound::Sound*	m_sound;

	IXAudio2*				m_xaudio;
	IXAudio2MasteringVoice*	m_masteringVoice;
	float	m_volume;
	bool	m_mute;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	Sound();

	/**
		@brief	�f�X�g���N�^
	*/
	~Sound();

	/**
		@brief	���������s���B
	*/
	bool Initialize( int32_t voiceCount1ch, int32_t voiceCount2ch );

	/**
		@brief	�f�o�C�X���擾����B
	*/
	IXAudio2* GetDevice();
	
	/**
		@brief	�{�����[���ݒ�B
	*/
	void SetVolume( float volume );

	/**
		@brief	�����ݒ�B
	*/
	void SetMute( bool mute );
	
	float GetVolume()	{return m_volume;}
	bool GetMute()		{return m_mute;}
	::EffekseerSound::Sound*	GetSound() { return m_sound; };
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERTOOL_SOUND_H__