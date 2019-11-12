#include	"ImGuiSeting.h"
#include	"KDL.h"
#include	"LoadAllFileName.h"
#include	"ObjectManager.h"
#include	"GameScene.h"

// �t�@�C���n�̍X�V
void SceneGame::FileOperateUpdate()
{
#if false
	constexpr float PrintTimeMax{ 1.f };

	const VF2 screen{ GMLIB->GetScreenSize() / 2.f };

	// �ۑ�
	if (file_flg.is_save)			SaveFile(screen, PrintTimeMax);

	// �ʖ��ۑ�
	//else if (file_flg.is_save_as)	SaveAsFile(screen, PrintTimeMax);

	// �J����
	else if (file_flg.is_open)		OpenFile(screen, PrintTimeMax);

	// ����
	//else if (file_flg.is_delete)	EraseFile(screen, PrintTimeMax);

	// ����
	//else if (file_flg.is_close)		CloseFile(screen, PrintTimeMax);

	// ���O�ύX
	//else if (file_flg.is_rename)	ReNameFile(screen, PrintTimeMax);

	// �R�s�[
	//else if (file_flg.is_copy)		CopyFileF(screen, PrintTimeMax);
#endif
}

// �ۑ�-------------------------------------------------------------------------------------------------------
void SceneGame::SaveFile(const VF2& screen, const float PrintTimeMax)
{
	static ImVec2 size{ 100.f, 30.f };
#ifdef KDL_USE_IMGUI
#if false
	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// ���ɊJ���Ă���
	if (!open_file_path.empty())
	{
		static bool output_first{ true };
		static bool is_success{ false };

		if (output_first)
		{
			is_success = Output(open_file_path);
			output_first = false;
		}

		// �����o������
		if (is_success)
		{
			static float timer;

			// ��莞�ԕ\��
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"�ۑ����܂����B");
			}
			else
			{
				timer = 0.f;
				file_flg.is_save = false;
				is_save = true;
				is_success = false;
				output_first = true;
			}
		}
		// �����o�����s
		else
		{
			static float timer;

			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"���s���܂����B");
			}
			else
			{
				timer = 0.f;
				file_flg.is_save = true;
				is_success = false;
				output_first = true;
			}
		}
	}
	// �J���Ă��Ȃ�
	else
	{
		static bool yes_save{ false };

		// �u�͂��v��I��������ɕ\�����Ȃ���
		if (!yes_save)
		{
			ImGui::Text(u8"�t�@�C�����J����Ă��܂���B�쐬���܂����H");

			// �͂�
			if (auto result{ ImguiTool::YesNoButton(u8"�͂�", u8"������") }; result.first)
			{
				yes_save = true;
			}
			// ������
			else if (result.second)
			{
				file_flg.is_save = false;
				is_save = false;
				yes_save = false;
			}
		}
		else
		{
			static bool go_save_file{ false };
			static const std::string InitFileNameData(64u, '\0');
			static std::string input_file_name(64u, '\0');

			// �쐬���ۑ�����
			if (yes_save && !go_save_file)
			{
				ImGui::TextColored({ IMGUI_RED },
					u8"�o�O��\���������ŁA�p��œ��͂��Ă�������");
				ImGui::InputText(u8"�t�@�C����", input_file_name.data(), input_file_name.size());

				// ���������͂��ꂽ���_�ŕ\��
				if (input_file_name.find('\0') != 0u)
				{
					static bool is_ok{ false };
					static bool confirm_same_file{ false };

					ImGui::Text(u8"��낵���ł����H");

					// �u�͂��v����������ɕ\�����Ȃ���
					if (!is_ok)
					{
						if (!go_save_file && !confirm_same_file)
						{
							ImGui::SameLine();
							if (ImGui::Button(u8"�͂�"))
							{
								// '\0'���폜����
								input_file_name.erase(
									input_file_name.find_first_of('\0'), input_file_name.size());

								if (!input_file_name.empty())
								{
									input_file_name =
										(SaveFileDir / input_file_name += FileExtension).string();

									is_ok = true;

									// �����t�@�C�������݂��邩�m�F
									if (std::filesystem::exists(input_file_name))
										confirm_same_file = true;
								}
							}
						}
					}

					// �u�͂��v����������
					if (is_ok)
					{
						// �����t�@�C�������݂���
						if (confirm_same_file)
						{
							ImguiTool::Spacing(5u);
							ImGui::Text(u8"�����t�@�C�������݂��܂�����낵���ł����H");

							// ����ZE��
							if (auto result{ ImguiTool::YesNoButton(u8"����ZE��", u8"����ZE��") }; result.first)
							{
								go_save_file = true;
								confirm_same_file = false;
								open_file_path.clear();
								is_ok = false;
							}
							// ����ZE��
							else if (result.second)
							{
								go_save_file = false;
								confirm_same_file = false;
								open_file_path.clear();
								is_ok = false;
								input_file_name = InitFileNameData;
							}
						}
						// ���Ȃ�
						else
						{
							go_save_file = true;
							open_file_path.clear();
							confirm_same_file = false;
							is_ok = false;
						}
					}
				}

				ImguiTool::Spacing(5u);
				if (ImGui::Button(u8"�L�����Z��"))
				{
					yes_save = false;
					go_save_file = false;
					input_file_name = InitFileNameData;
					file_flg.is_save = false;
				}
			}

			// �����o��
			if (go_save_file)
			{
				static bool output_first{ true };
				static bool is_success{ false };

				if (output_first)
				{
					Path path{ input_file_name };
					is_success = Output(path);
					output_first = false;
				}

				// �����o������
				if (is_success)
				{
					static float timer;

					// ��莞�ԕ\��
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"�쐬���ĕۑ����܂����B");
					}
					else
					{
						timer = 0.f;
						file_flg.is_save = false;
						is_save = true;
						go_save_file = false;
						yes_save = false;
						is_success = false;
						output_first = true;
						open_file_path = input_file_name;
						input_file_name = InitFileNameData;
						GameScene::is_save = false;
					}
				}
				// �����o�����s
				else
				{
					static float timer;
					static ImVec2 size{ 100.f, 30.f };

					// ��莞�ԕ\��
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"���s���܂����B");
					}
					else
					{
						timer = 0.f;
						file_flg.is_save = false;
						open_file_path.clear();
						go_save_file = false;
						yes_save = false;
						is_success = false;
						output_first = true;
						input_file_name = InitFileNameData;
					}
				}
			}
		}
	}

	ImGui::End();
#endif
#endif
}

// �ʖ��ۑ�---------------------------------------------------------------------------------------------------
#if false
void GameScene::SaveAsFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "SaveAs",
		true, 1.f);

	size = ImGui::GetWindowSize();

	static bool go_save_file{ false };
	static const std::string InitFileNameData(64u, '\0');
	static std::string input_file_name(64u, '\0');

	// �쐬���ۑ�����
	if (!go_save_file)
	{
		ImGui::TextColored(IMGUI_RED,
			u8"�o�O��\���������ŁA�p��œ��͂��Ă�������");
		ImGui::InputText(u8"�t�@�C����", input_file_name.data(), input_file_name.size());

		// ���������͂��ꂽ���_�ŕ\��
		if (input_file_name.find('\0') != 0u)
		{
			static bool is_ok{ false };
			static bool confirm_same_file{ false };

			ImGui::Text(u8"��낵���ł����H");

			// �u�͂��v����������ɕ\�����Ȃ���
			if (!is_ok)
			{
				if (!go_save_file && !confirm_same_file)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"�͂�"))
					{
						// '\0'���폜����
						input_file_name.erase(
							input_file_name.find_first_of('\0'), input_file_name.size());

						if (!input_file_name.empty())
						{
							input_file_name =
								(SaveFileDir / input_file_name += FileExtension).string();

							is_ok = true;

							// �����t�@�C�������݂��邩�m�F
							if (std::filesystem::exists(input_file_name))
								confirm_same_file = true;
						}
					}
				}
			}

			// �u�͂��v����������
			if (is_ok)
			{
				// �����t�@�C�������݂���
				if (confirm_same_file)
				{
					ImguiTool::Spacing(5u);
					ImGui::Text(u8"�����t�@�C�������݂��܂�����낵���ł����H");

					// ����ZE��
					if (auto result{ ImguiTool::YesNoButton(u8"����ZE��", u8"����ZE��") }; result.first)
					{
						go_save_file = true;
						confirm_same_file = false;
						open_file_path.clear();
						is_ok = false;
					}

					// ����ZE��
					else if (result.second)
					{
						go_save_file = false;
						confirm_same_file = false;
						open_file_path.clear();
						is_ok = false;
						input_file_name = InitFileNameData;
					}
				}
				// ���Ȃ�
				else
				{
					go_save_file = true;
					open_file_path.clear();
					confirm_same_file = false;
					is_ok = false;
				}
			}
		}

		ImguiTool::Spacing(5u);
		if (ImGui::Button(u8"�L�����Z��"))
		{
			go_save_file = false;
			input_file_name = InitFileNameData;
			file_flg.is_save_as = false;
		}
	}

	// �����o��
	if (go_save_file)
	{
		static bool output_first{ true };
		static bool is_success{ false };

		if (output_first)
		{
			Path path{ input_file_name };
			is_success = Output(path);
			output_first = false;
		}

		// �����o������
		if (is_success)
		{
			static float timer;

			// ��莞�ԕ\��
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"�쐬���ĕۑ����܂����B");
			}
			else
			{
				timer = 0.f;
				file_flg.is_save_as = false;
				editer_flg.is_save = true;
				go_save_file = false;
				is_success = false;
				output_first = true;
				open_file_path = input_file_name;
				input_file_name = InitFileNameData;
			}
		}
		// �����o�����s
		else
		{
			static float timer;
			static ImVec2 size{ 100.f, 30.f };

			// ��莞�ԕ\��
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"���s���܂����B");
			}
			else
			{
				timer = 0.f;
				file_flg.is_save_as = false;
				open_file_path.clear();
				go_save_file = false;
				is_success = false;
				output_first = true;
				input_file_name = InitFileNameData;
			}
		}
	}

	ImGui::End();

#endif
}
#endif

// �J��-------------------------------------------------------------------------------------------------------
#if true
void SceneGame::OpenFile(const VF2& screen, const float PrintTimeMax)
{
#ifdef KDL_USE_IMGUI
#if false
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// ���ɊJ���Ă���
	if (!open_file_path.empty())
	{
		static bool first{ true };
		static std::vector<Path> file_names;
		static std::unique_ptr<Object> temp_obj;

		// �ŏ�����
		if (first)
		{
			// �t�@�C���p�X�ɑ��݂���t�@�C���𑖍����擾
			file_names = GetAllFileName(SaveFileDir);

			// �g���q���������t�@�C�����𒊏o����
			for (auto& name : file_names)
				name = name.stem();

			temp_obj = std::make_unique<Object>((object_manager->GetObjectData()));

			first = false;
		}

		// �P�ł��t�@�C�������݂���
		if (!file_names.empty())
		{
			static Path select_item;

			ImguiTool::SelectVectorString(u8"�t�@�C���ꗗ", file_names, select_item);

			static bool is_file_opne_ok{ false };
			static std::pair<bool, Path> is_open_save_confirm{ false, "" };

			// �u�͂��v��I��������ɕ\�����Ȃ���
			if (!is_file_opne_ok)
			{
				if (!select_item.empty())
				{
					ImGui::Text(u8"�u%s�v���J���܂��B", select_item.u8string().data());

					ImGui::Text(u8"���Ƀt�@�C�����J���Ă��܂����A��낵���ł����H");

					if (!is_save)
						ImGui::TextColored({ IMGUI_RED }, u8"���݊J���Ă���t�@�C���͕ۑ�����܂����H");

					if (ImGui::Button(u8"�͂�"))
					{
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}

					if (!is_save && ImGui::Button(u8"�ۑ����ĊJ��"))
					{
						is_open_save_confirm = { true, open_file_path };
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}
				}

				if (ImGui::Button(u8"�L�����Z��"))
				{
					file_flg.is_open = false;
					is_file_opne_ok = false;
					first = true;
					file_names.clear();
					select_item.clear();
					is_open_save_confirm = { false, "" };
					temp_obj = nullptr;
				}
			}
			else
			{
				// �ۑ����ĊJ��
				if (is_open_save_confirm.first)
				{
					static bool output_first{ true };
					static bool is_out_success{ false };

					if (output_first)
					{
						is_out_success = Output(is_open_save_confirm.second);
						output_first = false;
					}

					// �����o������
					if (is_out_success)
					{
						static bool input_first{ true };
						static bool is_in_success{ false };

						if (input_first)
						{
							object_manager->GetObjectData().AllObjectClear();
							is_in_success = Input(select_item);
							input_first = false;
						}

						// �ǂݍ��ݐ���
						if (is_in_success)
						{
							is_file_opne_ok = false;
							file_flg = false;
							first = true;
							file_names.clear();
							open_file_path = select_item;
							select_item.clear();
							is_open_save_confirm = { false, "" };
							is_save = true;
							is_out_success = false;
							output_first = true;
							is_in_success = false;
							input_first = true;
							temp_obj = nullptr;
							//obj_stack.AddUnDo();  // UnDo�ɒǉ�
						}
						// �ǂݍ��ݎ��s
						else
						{
							static float timer;
							static ImVec2 size{ 100.f, 30.f };

							// ��莞�ԕ\��
							if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
							{
								ImGui::Text(u8"�J���܂���ł����B");
							}
							else
							{
								timer = 0.f;
								file_flg.is_open = false;
								is_file_opne_ok = false;
								first = true;
								file_names.clear();
								is_open_save_confirm = { false, "" };
								open_file_path = select_item;
								select_item.clear();
								is_out_success = false;
								output_first = true;
								is_in_success = false;
								input_first = true;
								object_manager->GetObjectData() = std::move(*temp_obj);
								temp_obj = nullptr;
							}
						}
					}
					// �����o�����s
					else
					{
						static float timer;
						static ImVec2 size{ 100.f, 30.f };

						// ��莞�ԕ\��
						if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
						{
							ImGui::Text(u8"���s���܂����B");
						}
						else
						{
							timer = 0.f;
							file_flg.is_open = false;
							is_file_opne_ok = false;
							first = true;
							file_names.clear();
							is_open_save_confirm = { false, "" };
							select_item.clear();
							is_out_success = false;
							output_first = true;
							temp_obj = nullptr;
						}
					}
				}
				// �J��
				else
				{
					static bool input_first{ true };
					static bool is_in_success{ false };

					if (input_first)
					{
						object_manager->GetObjectData().AllObjectClear();
						is_in_success = Input(select_item);
						input_first = false;
					}

					// �ǂݍ��ݐ���
					if (is_in_success)
					{
						is_file_opne_ok = false;
						file_flg = false;
						first = true;
						file_names.clear();
						is_save = true;
						is_in_success = false;
						input_first = true;
						temp_obj = nullptr;
						open_file_path = select_item;
						select_item.clear();
						//obj_stack.AddUnDo();  // UnDo�ɒǉ�
					}
					// �ǂݍ��ݎ��s
					else
					{
						static float timer;
						static ImVec2 size{ 100.f, 30.f };

						// ��莞�ԕ\��
						if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
						{
							ImGui::Text(u8"�J���܂���ł����B");
						}
						else
						{
							timer = 0.f;
							file_flg.is_open = false;
							is_file_opne_ok = false;
							first = true;
							file_names.clear();
							select_item.clear();
							is_in_success = false;
							input_first = true;
							open_file_path = select_item;
							select_item.clear();
							object_manager->GetObjectData() = std::move(*temp_obj);
							temp_obj = nullptr;
						}
					}
				}
			}
		}
		// ��������
		else
		{
			static float timer;

			// ��莞�ԕ\��
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::TextColored({ IMGUI_RED }, u8"�t�@�C�������݂��Ȃ�");
			}
			else
			{
				timer = 0.f;
				file_flg.is_open = false;
				first = true;
				file_names.clear();
				temp_obj = nullptr;
			}
		}
	}
	// �J���Ă��Ȃ�
	else
	{
		static bool first{ true };
		static std::vector<Path> file_names;
		static std::unique_ptr<Object> temp_obj;

		// �ŏ�����
		if (first)
		{
			// �t�@�C���p�X�ɑ��݂���t�@�C���𑖍����擾
			file_names = GetAllFileName(SaveFileDir);

			// �g���q���������t�@�C�����𒊏o����
			for (auto& name : file_names)
				name = name.stem();

			temp_obj = std::make_unique<Object>((object_manager->GetObjectData()));

			first = false;
		}

		// �P�ł��t�@�C�������݂���
		if (!file_names.empty())
		{
			static Path select_item;

			ImguiTool::SelectVectorString(u8"�t�@�C���ꗗ", file_names, select_item);

			static bool is_file_opne_ok{ false };

			// �u�͂��v��I��������ɕ\�����Ȃ���
			if (!is_file_opne_ok)
			{
				if (!select_item.empty())
				{
					ImGui::Text(u8"�u%s�v���J���܂��B", select_item.u8string().data());

					if (!is_save)
						ImGui::TextColored({ IMGUI_RED }, u8"���݂̏󋵂͕ۑ�����܂����H");

					if (ImGui::Button(u8"�͂�"))
					{
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}
				}

				if (ImGui::Button(u8"�L�����Z��"))
				{
					file_flg.is_open = false;
					is_file_opne_ok = false;
					first = true;
					file_names.clear();
					select_item.clear();
				}
			}
			else
			{
				static bool input_first{ true };
				static bool is_in_success{ false };

				// �ŏ�����
				if (input_first)
				{
					object_manager->GetObjectData().AllObjectClear();
					is_in_success = Input(select_item);
					input_first = false;
				}

				// �ǂݍ��ݐ���
				if (is_in_success)
				{
					file_flg = false;
					is_file_opne_ok = false;
					first = true;
					is_save = true;
					file_names.clear();
					open_file_path = select_item;
					select_item.clear();
					is_in_success = false;
					input_first = true;
					temp_obj = nullptr;
					GameScene::is_save = false;
					//obj_stack.AddUnDo();  // UnDo�ɒǉ�
				}
				// �ǂݍ��ݎ��s
				else
				{
					static float timer;

					// ��莞�ԕ\��
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"�J���܂���ł����B");
					}
					else
					{
						timer = 0.f;
						file_flg.is_open = false;
						is_file_opne_ok = false;
						first = true;
						file_names.clear();
						select_item.clear();
						is_in_success = false;
						input_first = true;
						object_manager->GetObjectData() = std::move(*temp_obj);
						temp_obj = nullptr;
					}
				}
			}
		}
		// ��������
		else
		{
			static float timer;

			// ��莞�ԕ\��
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::TextColored({ IMGUI_RED }, u8"�t�@�C�������݂��Ȃ�");
			}
			else
			{
				timer = 0.f;
				file_flg.is_open = false;
				first = true;
				file_names.clear();
				temp_obj = nullptr;
			}
		}
	}

	ImGui::End();
#endif
#endif
}
#endif

// �폜-------------------------------------------------------------------------------------------------------
#if false
void GameScene::EraseFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Erase",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// �J����Ă��Ȃ�
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"�t�@�C�����J���Ă��܂���");
		}
		else
		{
			file_flg.is_delete = false;
			timer = 0.0;
		}
	}
	// �J����Ă���
	else
	{
		static bool first_print{ true };
		static bool is_confirm_ok{ false };

		// �u�͂��v��ɕ\�����Ȃ���
		if (first_print)
		{
			ImGui::Text(u8"���݂̊J����Ă���t�@�C�����폜���܂��B��낵���ł��ˁH");

			// ������`
			if (auto result{ ImguiTool::YesNoButton(u8"������`", u8"�������`��") }; result.first)
			{
				first_print = false;
			}
			// �������`��
			else if (result.second)
			{
				file_flg.is_delete = false;
			}
		}
		else
		{
			static bool success_remove{ false };
			static bool remove_first_time{ true };

			// ���񂾂�����
			if (remove_first_time)
			{
				success_remove = std::filesystem::remove(open_file_path);

				remove_first_time = false;
			}
			else
			{
				// �폜�ɐ���
				if (success_remove)
				{
					static double timer{ 0.0 };

					// ��莞�ԕ\��
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"�폜���܂����B");
					}
					else
					{
						InitObjectClear();
						file_flg = false;
						open_file_path.clear();
						timer = 0.0;
						remove_first_time = true;
						success_remove = false;
						is_confirm_ok = false;
						first_print = true;
					}
				}
				// �폜�Ɏ��s
				else
				{
					static double timer{ 0.0 };

					// ��莞�ԕ\��
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"�폜�Ɏ��s���܂����B");
					}
					else
					{
						file_flg.is_delete = false;
						timer = 0.0;
						remove_first_time = true;
						success_remove = false;
						is_confirm_ok = false;
						first_print = true;
					}
				}
			}
		}
	}

	ImGui::End();
#endif
}
#endif

// ����-----------------------------------------------------------------------------------------------------
#if false
void GameScene::CloseFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Close",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// �t�@�C�����J���Ă��Ȃ�
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"�t�@�C�����J���Ă��Ȃ�");
		}
		else
		{
			file_flg.is_close = false;
			timer = 0.0;
		}
	}
	// �t�@�C�����J���Ă���
	else
	{
		ImGui::Text(u8"�t�@�C������܂��B��낵���ł����H");

		if (!editer_flg.is_save)
			ImGui::TextColored(IMGUI_RED, u8"�ۑ�����Ă܂����H�����́H");

		// �͂��I
		if (auto result{ ImguiTool::YesNoButton(u8"�͂��I", u8"����I") }; result.first)
		{
			InitObjectClear();
			open_file_path.clear();
			file_flg = false;
		}
		// ����I
		else if (result.second)
		{
			file_flg.is_close = false;
		}
	}

	ImGui::End();
#endif
}
#endif

// ���O�ύX---------------------------------------------------------------------------------------------------
#if false
void GameScene::ReNameFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "ReName",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// �t�@�C�����J���Ă��Ȃ�
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"�t�@�C�����J���Ă��Ȃ�");
		}
		else
		{
			file_flg.is_close = false;
			timer = 0.0;
		}
	}
	// �t�@�C�����J���Ă���
	else
	{
		static const std::string InitFileNameData(64u, '\0');
		static std::string input_file_name(64u, '\0');
		static bool is_input_ok{ false };

		if (!is_input_ok)
		{
			ImGui::TextColored(IMGUI_RED,
				u8"�o�O��\���������ŁA�p��œ��͂��Ă�������");
			ImGui::InputText(u8"�V�����t�@�C����", input_file_name.data(), input_file_name.size());
		}

		// ���������͂��ꂽ���_�ŕ\��
		if (input_file_name.find('\0') != 0u)
		{
			static bool is_ok{ false };
			static bool confirm_same_file{ false };

			// �u�͂��v����������ɕ\�����Ȃ���
			if (!is_ok)
			{
				ImGui::Text(u8"��낵���ł����H");

				if (!confirm_same_file)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"�͂�"))
					{
						// '\0'���폜����
						input_file_name.erase(
							input_file_name.find_first_of('\0'), input_file_name.size());

						if (!input_file_name.empty())
						{
							input_file_name =
								(SaveFileDir / input_file_name += FileExtension).string();

							is_ok = true;
							is_input_ok = true;

							// �����t�@�C�������݂��邩�m�F
							if (std::filesystem::exists(input_file_name))
								confirm_same_file = true;
						}
					}
				}

				ImguiTool::Spacing(5u);
				if (ImGui::Button(u8"�L�����Z��"))
				{
					input_file_name = InitFileNameData;
					file_flg.is_rename = false;
					confirm_same_file = false;
					is_ok = false;
					is_input_ok = false;
				}
			}
			// �u�͂��v����������
			else
			{
				static bool is_confirm_ok{ false };

				// �uYES!YES!�v����������\�����Ȃ���
				if (!is_confirm_ok)
				{
					// �����t�@�C�������݂���
					if (confirm_same_file)
					{
						ImguiTool::Spacing(5u);
						ImGui::Text(u8"�������O�ł����A��낵���ł����H");

						// YES!YES!
						if (auto result{ ImguiTool::YesNoButton("YES!YES!", u8"NO!NO!") }; result.first)
						{
							is_confirm_ok = true;
						}
						// NO!NO!
						else if (result.second)
						{
							confirm_same_file = false;
							is_ok = false;
							input_file_name = InitFileNameData;
							is_confirm_ok = false;
						}
					}
					// ���Ȃ�
					else
					{
						is_confirm_ok = true;
					}
				}
				else
				{
					static double timer{ 0.0 };
					static bool first{ true };

					// ���񂾂�
					if (first)
					{
						// ���ۂɖ��O�ύX����
						std::filesystem::rename(open_file_path, input_file_name);

						first = false;
					}

					// ��莞�ԕ\��
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"�ύX���܂���");
					}
					else
					{
						timer = 0.0;
						confirm_same_file = false;
						file_flg.is_rename = false;
						first = true;
						is_ok = false;
						open_file_path = input_file_name;
						input_file_name = InitFileNameData;
						is_confirm_ok = false;
						is_input_ok = false;
					}
				}
			}
		}
	}

	ImGui::End();

#endif
}
#endif

// �R�s�[---------------------------------------------------------------------------------------------------
#if false
void GameScene::CopyFileF(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// �t�@�C�����J����Ă��Ȃ�
	if (open_file_path.empty())
	{
		static double timer;

		// ��莞�ԕ\��
		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"�t�@�C�����J����Ă��܂���B");
		}
		else
		{
			timer = 0.0;
			file_flg.is_copy = false;
		}
	}
	// �t�@�C�����J����Ă���
	else
	{
		constexpr std::pair<bool, bool> init_pair_data{ false, false };

		static bool confirm_is_ok{ false };
		static std::pair<bool, bool> copy_file_kinds{ init_pair_data };
		static bool first_time{ true };
		static std::vector<Path> file_names;
		static Path select_item;

		// �ŏ�����
		if (first_time)
		{
			// �t�@�C���p�X�ɑ��݂���t�@�C���𑖍����擾
			file_names = GetAllFileName(SaveFileDir);

			// �O���b�g��ۑ����Ă���E�J���Ă���t�@�C�����폜
			file_names.erase(std::remove_if(file_names.begin(), file_names.end(),
				[&](Path& path) { return (Grit::FileDir == path || open_file_path == path); }),
				file_names.end());

			// �g���q���������t�@�C�����𒊏o����
			for (auto& name : file_names)
				name = name.stem();

			first_time = false;
		}

		// �u�͂��v����������A�\�����Ȃ���
		if (!confirm_is_ok)
		{
			// �I��������\�����Ȃ���
			if (copy_file_kinds == init_pair_data)
			{
				ImGui::Text(u8"�R�s�[�����ނ�I��ŉ������B");

				copy_file_kinds =
					ImguiTool::YesNoButton(u8"�u�J���Ă���t�@�C���v���u�ʂ̃t�@�C���v�փR�s�[",
						u8"�u�ʂ̃t�@�C���v���u�J���Ă���t�@�C���v�փR�s�[");
			}
			// �ʂ̃t�@�C���փR�s�[
			else if (copy_file_kinds.first)
			{
				ImGui::Text(u8"%s����", open_file_path.stem().u8string().data());
				ImGui::SameLine();
				ImguiTool::SelectVectorString(u8"�ɃR�s�[����", file_names, select_item);

				if (!editer_flg.is_save)
					ImGui::TextColored(IMGUI_RED,
						u8"�ۑ�����Ă܂����H������������H");

				if (!select_item.empty())
				{
					ImGui::Text("AreYouOK?");

					if (ImGui::Button(u8"�n�C�I"))
					{
						confirm_is_ok = true;
					}
				}
			}
			// �ʂ̃t�@�C������R�s�[
			else if (copy_file_kinds.second)
			{
				ImguiTool::SelectVectorString(u8"����", file_names, select_item);
				ImGui::SameLine();
				ImGui::Text(u8"%s�փR�s�[����", open_file_path.stem().u8string().data());

				if (!editer_flg.is_save)
				{
					ImGui::TextColored(IMGUI_RED,
						u8"�ʂ̃t�@�C������R�s�[�����ŁA���̃f�[�^�͏����܂�����I");
					ImGui::TextColored(IMGUI_RED,
						u8"���̌�A�ǂݍ��܂���Ō��݂̃I�u�W�F�N�g�����S�ɏ����܂�����!?");
				}

				if (!select_item.empty())
				{
					ImGui::Text("AreYouOK?");

					if (ImGui::Button(u8"�n�C�I"))
					{
						confirm_is_ok = true;
					}
				}
			}

			ImguiTool::Spacing(5u);
			if (ImGui::Button(u8"�L�����Z��"))
			{
				copy_file_kinds = init_pair_data;
				confirm_is_ok = false;
				file_flg.is_copy = false;
				select_item.clear();
				file_names.clear();
				first_time = true;
				confirm_is_ok = false;
			}
		}
		else
		{
			static bool first{ true };
			static bool success_copy{ false };
			static Path other_file_path;

			// ���񂾂�
			if (first)
			{
				using std::filesystem::copy_file;

				constexpr auto Option{ std::filesystem::copy_options::overwrite_existing };

				other_file_path = (SaveFileDir / select_item) += FileExtension;

				// �ʂ̃t�@�C���փR�s�[
				if (copy_file_kinds.first)
				{
					success_copy = copy_file(open_file_path, other_file_path, Option);
				}
				// �ʂ̃t�@�C������R�s�[
				else if (copy_file_kinds.second)
				{
					success_copy = copy_file(other_file_path, open_file_path, Option);

					Objects temp{ objects };  // �ǂݍ��ݎ��s�����ۂɖ߂��ׂ̃f�[�^

					InitObjectClear();

					if (success_copy)
						success_copy = Input(other_file_path);
					else
						objects = std::move(temp);
				}

				first = false;
			}
			// ���ڈȍ~
			else
			{
				// �R�s�[����
				if (success_copy)
				{
					static double timer;

					// ��莞�ԕ\��
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						// �ʂ̃t�@�C���փR�s�[
						if (copy_file_kinds.first)
						{
							ImGui::Text(u8"�u%s�v�փR�s�[���܂����B", other_file_path.u8string().data());
						}
						// �ʂ̃t�@�C������R�s�[
						else if (copy_file_kinds.second)
						{
							ImGui::Text(u8"�u%s�v����R�s�[���܂����B", other_file_path.u8string().data());
						}
					}
					else
					{
						// ������
						other_file_path.clear();
						timer = 0.0;
						file_flg = false;
						confirm_is_ok = false;
						success_copy = false;
						first = true;
						first_time = true;
						select_item.clear();
						file_names.clear();
						copy_file_kinds = init_pair_data;
					}
				}
				// �R�s�[���s
				else
				{
					static double timer;

					// ��莞�ԕ\��
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"�R�s�[�Ɏ��s���܂����B");
					}
					else
					{
						timer = 0.0;
						file_flg.is_copy = false;
						success_copy = false;
						first = true;
						confirm_is_ok = false;
						first_time = true;
						select_item.clear();
						file_names.clear();
						other_file_path.clear();
						copy_file_kinds = init_pair_data;
					}
				}
			}
		}
	}

	ImGui::End();
#endif
}
#endif
