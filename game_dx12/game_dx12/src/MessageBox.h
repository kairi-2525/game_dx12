#pragma once

#include <WinUser.h>

enum class BoxType : UINT
{
	Abort_Retry_Ignore = MB_ABORTRETRYIGNORE,
	Ok                 = MB_OK,
	Ok_Cancel          = MB_OKCANCEL,
	Retry_Cancel       = MB_RETRYCANCEL,
	Yes_No             = MB_YESNO,
	Yes_No_Cancel      = MB_YESNOCANCEL,
};

enum class BoxIcon : UINT
{
	Exclamation = MB_ICONEXCLAMATION,
	Warning     = MB_ICONINFORMATION,
	Information = MB_ICONINFORMATION,
	Asterisk    = MB_ICONASTERISK,
	Question    = MB_ICONQUESTION,
	Stop        = MB_ICONSTOP,
	Error       = MB_ICONERROR,
	Hand        = MB_ICONHAND,
};

static inline UINT operator|(const BoxType type, const BoxIcon icon)
{
	return static_cast<UINT>(type) | static_cast<UINT>(icon);
}

enum class BoxReturn : INT
{
	Yes_Ok = IDOK,
	No     = IDNO,
	Cancel = IDCANCEL,
	Retry  = IDRETRY,
	Ignore = IDIGNORE,
};

// ���b�Z�[�W�{�b�N�X��\������
// �����F���C���̃e�L�X�g�A�t���[�������ɕ\������e�L�X�g�A�{�b�N�X�̕\���`��
// �߂�l�F�{�b�N�X�̕\���`���ɉ������߂�l
BoxReturn Message(const wchar_t* text, const wchar_t* frame_text = nullptr,
	const UINT type = static_cast<UINT>(BoxType::Ok))
{
	int rv{ MessageBox(nullptr, text, frame_text, type) };

	if (rv == IDYES || rv == IDOK)
		return BoxReturn::Yes_Ok;
	else
	{
		return static_cast<BoxReturn>(rv);
	}
}