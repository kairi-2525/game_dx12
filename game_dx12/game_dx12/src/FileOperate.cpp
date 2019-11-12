#include	"ImGuiSeting.h"
#include	"KDL.h"
#include	"LoadAllFileName.h"
#include	"ObjectManager.h"
#include	"GameScene.h"

// ファイル系の更新
void SceneGame::FileOperateUpdate()
{
#if false
	constexpr float PrintTimeMax{ 1.f };

	const VF2 screen{ GMLIB->GetScreenSize() / 2.f };

	// 保存
	if (file_flg.is_save)			SaveFile(screen, PrintTimeMax);

	// 別名保存
	//else if (file_flg.is_save_as)	SaveAsFile(screen, PrintTimeMax);

	// 開ける
	else if (file_flg.is_open)		OpenFile(screen, PrintTimeMax);

	// 消去
	//else if (file_flg.is_delete)	EraseFile(screen, PrintTimeMax);

	// 閉じる
	//else if (file_flg.is_close)		CloseFile(screen, PrintTimeMax);

	// 名前変更
	//else if (file_flg.is_rename)	ReNameFile(screen, PrintTimeMax);

	// コピー
	//else if (file_flg.is_copy)		CopyFileF(screen, PrintTimeMax);
#endif
}

// 保存-------------------------------------------------------------------------------------------------------
void SceneGame::SaveFile(const VF2& screen, const float PrintTimeMax)
{
	static ImVec2 size{ 100.f, 30.f };
#ifdef KDL_USE_IMGUI
#if false
	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// 既に開いている
	if (!open_file_path.empty())
	{
		static bool output_first{ true };
		static bool is_success{ false };

		if (output_first)
		{
			is_success = Output(open_file_path);
			output_first = false;
		}

		// 書き出し成功
		if (is_success)
		{
			static float timer;

			// 一定時間表示
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"保存しました。");
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
		// 書き出し失敗
		else
		{
			static float timer;

			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"失敗しました。");
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
	// 開いていない
	else
	{
		static bool yes_save{ false };

		// 「はい」を選択した後に表示しない為
		if (!yes_save)
		{
			ImGui::Text(u8"ファイルが開かれていません。作成しますか？");

			// はい
			if (auto result{ ImguiTool::YesNoButton(u8"はい", u8"いいえ") }; result.first)
			{
				yes_save = true;
			}
			// いいえ
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

			// 作成し保存する
			if (yes_save && !go_save_file)
			{
				ImGui::TextColored({ IMGUI_RED },
					u8"バグる可能性があるんで、英語で入力してください");
				ImGui::InputText(u8"ファイル名", input_file_name.data(), input_file_name.size());

				// 何かが入力された時点で表示
				if (input_file_name.find('\0') != 0u)
				{
					static bool is_ok{ false };
					static bool confirm_same_file{ false };

					ImGui::Text(u8"よろしいですか？");

					// 「はい」を押した後に表示しない為
					if (!is_ok)
					{
						if (!go_save_file && !confirm_same_file)
						{
							ImGui::SameLine();
							if (ImGui::Button(u8"はい"))
							{
								// '\0'を削除する
								input_file_name.erase(
									input_file_name.find_first_of('\0'), input_file_name.size());

								if (!input_file_name.empty())
								{
									input_file_name =
										(SaveFileDir / input_file_name += FileExtension).string();

									is_ok = true;

									// 同じファイルが存在するか確認
									if (std::filesystem::exists(input_file_name))
										confirm_same_file = true;
								}
							}
						}
					}

					// 「はい」を押した後
					if (is_ok)
					{
						// 同じファイルが存在する
						if (confirm_same_file)
						{
							ImguiTool::Spacing(5u);
							ImGui::Text(u8"同じファイルが存在しますがよろしいですか？");

							// いいZE☆
							if (auto result{ ImguiTool::YesNoButton(u8"いいZE☆", u8"嫌だZE☆") }; result.first)
							{
								go_save_file = true;
								confirm_same_file = false;
								open_file_path.clear();
								is_ok = false;
							}
							// 嫌だZE☆
							else if (result.second)
							{
								go_save_file = false;
								confirm_same_file = false;
								open_file_path.clear();
								is_ok = false;
								input_file_name = InitFileNameData;
							}
						}
						// しない
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
				if (ImGui::Button(u8"キャンセル"))
				{
					yes_save = false;
					go_save_file = false;
					input_file_name = InitFileNameData;
					file_flg.is_save = false;
				}
			}

			// 書き出し
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

				// 書き出し成功
				if (is_success)
				{
					static float timer;

					// 一定時間表示
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"作成して保存しました。");
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
				// 書き出し失敗
				else
				{
					static float timer;
					static ImVec2 size{ 100.f, 30.f };

					// 一定時間表示
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"失敗しました。");
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

// 別名保存---------------------------------------------------------------------------------------------------
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

	// 作成し保存する
	if (!go_save_file)
	{
		ImGui::TextColored(IMGUI_RED,
			u8"バグる可能性があるんで、英語で入力してください");
		ImGui::InputText(u8"ファイル名", input_file_name.data(), input_file_name.size());

		// 何かが入力された時点で表示
		if (input_file_name.find('\0') != 0u)
		{
			static bool is_ok{ false };
			static bool confirm_same_file{ false };

			ImGui::Text(u8"よろしいですか？");

			// 「はい」を押した後に表示しない為
			if (!is_ok)
			{
				if (!go_save_file && !confirm_same_file)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"はい"))
					{
						// '\0'を削除する
						input_file_name.erase(
							input_file_name.find_first_of('\0'), input_file_name.size());

						if (!input_file_name.empty())
						{
							input_file_name =
								(SaveFileDir / input_file_name += FileExtension).string();

							is_ok = true;

							// 同じファイルが存在するか確認
							if (std::filesystem::exists(input_file_name))
								confirm_same_file = true;
						}
					}
				}
			}

			// 「はい」を押した後
			if (is_ok)
			{
				// 同じファイルが存在する
				if (confirm_same_file)
				{
					ImguiTool::Spacing(5u);
					ImGui::Text(u8"同じファイルが存在しますがよろしいですか？");

					// いいZE☆
					if (auto result{ ImguiTool::YesNoButton(u8"いいZE☆", u8"嫌だZE☆") }; result.first)
					{
						go_save_file = true;
						confirm_same_file = false;
						open_file_path.clear();
						is_ok = false;
					}

					// 嫌だZE☆
					else if (result.second)
					{
						go_save_file = false;
						confirm_same_file = false;
						open_file_path.clear();
						is_ok = false;
						input_file_name = InitFileNameData;
					}
				}
				// しない
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
		if (ImGui::Button(u8"キャンセル"))
		{
			go_save_file = false;
			input_file_name = InitFileNameData;
			file_flg.is_save_as = false;
		}
	}

	// 書き出し
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

		// 書き出し成功
		if (is_success)
		{
			static float timer;

			// 一定時間表示
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"作成して保存しました。");
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
		// 書き出し失敗
		else
		{
			static float timer;
			static ImVec2 size{ 100.f, 30.f };

			// 一定時間表示
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::Text(u8"失敗しました。");
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

// 開く-------------------------------------------------------------------------------------------------------
#if true
void SceneGame::OpenFile(const VF2& screen, const float PrintTimeMax)
{
#ifdef KDL_USE_IMGUI
#if false
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// 既に開いている
	if (!open_file_path.empty())
	{
		static bool first{ true };
		static std::vector<Path> file_names;
		static std::unique_ptr<Object> temp_obj;

		// 最初だけ
		if (first)
		{
			// ファイルパスに存在するファイルを走査し取得
			file_names = GetAllFileName(SaveFileDir);

			// 拡張子を除いたファイル名を抽出する
			for (auto& name : file_names)
				name = name.stem();

			temp_obj = std::make_unique<Object>((object_manager->GetObjectData()));

			first = false;
		}

		// １つでもファイルが存在する
		if (!file_names.empty())
		{
			static Path select_item;

			ImguiTool::SelectVectorString(u8"ファイル一覧", file_names, select_item);

			static bool is_file_opne_ok{ false };
			static std::pair<bool, Path> is_open_save_confirm{ false, "" };

			// 「はい」を選択した後に表示しない為
			if (!is_file_opne_ok)
			{
				if (!select_item.empty())
				{
					ImGui::Text(u8"「%s」を開きます。", select_item.u8string().data());

					ImGui::Text(u8"既にファイルを開いていますが、よろしいですか？");

					if (!is_save)
						ImGui::TextColored({ IMGUI_RED }, u8"現在開いているファイルは保存されませんよ？");

					if (ImGui::Button(u8"はい"))
					{
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}

					if (!is_save && ImGui::Button(u8"保存して開く"))
					{
						is_open_save_confirm = { true, open_file_path };
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}
				}

				if (ImGui::Button(u8"キャンセル"))
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
				// 保存して開く
				if (is_open_save_confirm.first)
				{
					static bool output_first{ true };
					static bool is_out_success{ false };

					if (output_first)
					{
						is_out_success = Output(is_open_save_confirm.second);
						output_first = false;
					}

					// 書き出し成功
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

						// 読み込み成功
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
							//obj_stack.AddUnDo();  // UnDoに追加
						}
						// 読み込み失敗
						else
						{
							static float timer;
							static ImVec2 size{ 100.f, 30.f };

							// 一定時間表示
							if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
							{
								ImGui::Text(u8"開けませんでした。");
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
					// 書き出し失敗
					else
					{
						static float timer;
						static ImVec2 size{ 100.f, 30.f };

						// 一定時間表示
						if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
						{
							ImGui::Text(u8"失敗しました。");
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
				// 開く
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

					// 読み込み成功
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
						//obj_stack.AddUnDo();  // UnDoに追加
					}
					// 読み込み失敗
					else
					{
						static float timer;
						static ImVec2 size{ 100.f, 30.f };

						// 一定時間表示
						if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
						{
							ImGui::Text(u8"開けませんでした。");
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
		// 何も無い
		else
		{
			static float timer;

			// 一定時間表示
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::TextColored({ IMGUI_RED }, u8"ファイルが存在しない");
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
	// 開いていない
	else
	{
		static bool first{ true };
		static std::vector<Path> file_names;
		static std::unique_ptr<Object> temp_obj;

		// 最初だけ
		if (first)
		{
			// ファイルパスに存在するファイルを走査し取得
			file_names = GetAllFileName(SaveFileDir);

			// 拡張子を除いたファイル名を抽出する
			for (auto& name : file_names)
				name = name.stem();

			temp_obj = std::make_unique<Object>((object_manager->GetObjectData()));

			first = false;
		}

		// １つでもファイルが存在する
		if (!file_names.empty())
		{
			static Path select_item;

			ImguiTool::SelectVectorString(u8"ファイル一覧", file_names, select_item);

			static bool is_file_opne_ok{ false };

			// 「はい」を選択した後に表示しない為
			if (!is_file_opne_ok)
			{
				if (!select_item.empty())
				{
					ImGui::Text(u8"「%s」を開きます。", select_item.u8string().data());

					if (!is_save)
						ImGui::TextColored({ IMGUI_RED }, u8"現在の状況は保存されませんよ？");

					if (ImGui::Button(u8"はい"))
					{
						is_file_opne_ok = true;
						select_item = SaveFileDir / select_item += FileExtension;
					}
				}

				if (ImGui::Button(u8"キャンセル"))
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

				// 最初だけ
				if (input_first)
				{
					object_manager->GetObjectData().AllObjectClear();
					is_in_success = Input(select_item);
					input_first = false;
				}

				// 読み込み成功
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
					//obj_stack.AddUnDo();  // UnDoに追加
				}
				// 読み込み失敗
				else
				{
					static float timer;

					// 一定時間表示
					if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
					{
						ImGui::Text(u8"開けませんでした。");
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
		// 何も無い
		else
		{
			static float timer;

			// 一定時間表示
			if (timer += GMLIB->GetElapsedTime(); timer < PrintTimeMax)
			{
				ImGui::TextColored({ IMGUI_RED }, u8"ファイルが存在しない");
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

// 削除-------------------------------------------------------------------------------------------------------
#if false
void GameScene::EraseFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Erase",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// 開かれていない
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"ファイルを開いていません");
		}
		else
		{
			file_flg.is_delete = false;
			timer = 0.0;
		}
	}
	// 開かれている
	else
	{
		static bool first_print{ true };
		static bool is_confirm_ok{ false };

		// 「はい」後に表示しない為
		if (first_print)
		{
			ImGui::Text(u8"現在の開かれているファイルを削除します。よろしいですね？");

			// いいよ～
			if (auto result{ ImguiTool::YesNoButton(u8"いいよ～", u8"あかぁ～ん") }; result.first)
			{
				first_print = false;
			}
			// あかぁ～ん
			else if (result.second)
			{
				file_flg.is_delete = false;
			}
		}
		else
		{
			static bool success_remove{ false };
			static bool remove_first_time{ true };

			// 初回だけ処理
			if (remove_first_time)
			{
				success_remove = std::filesystem::remove(open_file_path);

				remove_first_time = false;
			}
			else
			{
				// 削除に成功
				if (success_remove)
				{
					static double timer{ 0.0 };

					// 一定時間表示
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"削除しました。");
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
				// 削除に失敗
				else
				{
					static double timer{ 0.0 };

					// 一定時間表示
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"削除に失敗しました。");
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

// 閉じる-----------------------------------------------------------------------------------------------------
#if false
void GameScene::CloseFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Close",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// ファイルを開いていない
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"ファイルを開いていない");
		}
		else
		{
			file_flg.is_close = false;
			timer = 0.0;
		}
	}
	// ファイルを開いている
	else
	{
		ImGui::Text(u8"ファイルを閉じます。よろしいですか？");

		if (!editer_flg.is_save)
			ImGui::TextColored(IMGUI_RED, u8"保存されてませんよ？いいの？");

		// はい！
		if (auto result{ ImguiTool::YesNoButton(u8"はい！", u8"いや！") }; result.first)
		{
			InitObjectClear();
			open_file_path.clear();
			file_flg = false;
		}
		// いや！
		else if (result.second)
		{
			file_flg.is_close = false;
		}
	}

	ImGui::End();
#endif
}
#endif

// 名前変更---------------------------------------------------------------------------------------------------
#if false
void GameScene::ReNameFile(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "ReName",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// ファイルを開いていない
	if (open_file_path.empty())
	{
		static double timer;

		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"ファイルを開いていない");
		}
		else
		{
			file_flg.is_close = false;
			timer = 0.0;
		}
	}
	// ファイルを開いている
	else
	{
		static const std::string InitFileNameData(64u, '\0');
		static std::string input_file_name(64u, '\0');
		static bool is_input_ok{ false };

		if (!is_input_ok)
		{
			ImGui::TextColored(IMGUI_RED,
				u8"バグる可能性があるんで、英語で入力してください");
			ImGui::InputText(u8"新しいファイル名", input_file_name.data(), input_file_name.size());
		}

		// 何かが入力された時点で表示
		if (input_file_name.find('\0') != 0u)
		{
			static bool is_ok{ false };
			static bool confirm_same_file{ false };

			// 「はい」を押した後に表示しない為
			if (!is_ok)
			{
				ImGui::Text(u8"よろしいですか？");

				if (!confirm_same_file)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"はい"))
					{
						// '\0'を削除する
						input_file_name.erase(
							input_file_name.find_first_of('\0'), input_file_name.size());

						if (!input_file_name.empty())
						{
							input_file_name =
								(SaveFileDir / input_file_name += FileExtension).string();

							is_ok = true;
							is_input_ok = true;

							// 同じファイルが存在するか確認
							if (std::filesystem::exists(input_file_name))
								confirm_same_file = true;
						}
					}
				}

				ImguiTool::Spacing(5u);
				if (ImGui::Button(u8"キャンセル"))
				{
					input_file_name = InitFileNameData;
					file_flg.is_rename = false;
					confirm_same_file = false;
					is_ok = false;
					is_input_ok = false;
				}
			}
			// 「はい」を押した後
			else
			{
				static bool is_confirm_ok{ false };

				// 「YES!YES!」を押した後表示しない為
				if (!is_confirm_ok)
				{
					// 同じファイルが存在する
					if (confirm_same_file)
					{
						ImguiTool::Spacing(5u);
						ImGui::Text(u8"同じ名前ですが、よろしいですか？");

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
					// しない
					else
					{
						is_confirm_ok = true;
					}
				}
				else
				{
					static double timer{ 0.0 };
					static bool first{ true };

					// 初回だけ
					if (first)
					{
						// 実際に名前変更する
						std::filesystem::rename(open_file_path, input_file_name);

						first = false;
					}

					// 一定時間表示
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"変更しました");
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

// コピー---------------------------------------------------------------------------------------------------
#if false
void GameScene::CopyFileF(const VF2& screen, const float PrintTimeMax)
{
#if USE_IMGUI
	static ImVec2 size{ 100.f, 30.f };

	ImguiTool::BeginShowTempWindow({ screen.x - size.x / 2.f, screen.y - size.y / 2.f }, "Save",
		true, 1.f);

	size = ImGui::GetWindowSize();

	// ファイルが開かれていない
	if (open_file_path.empty())
	{
		static double timer;

		// 一定時間表示
		if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
		{
			ImGui::Text(u8"ファイルが開かれていません。");
		}
		else
		{
			timer = 0.0;
			file_flg.is_copy = false;
		}
	}
	// ファイルが開かれている
	else
	{
		constexpr std::pair<bool, bool> init_pair_data{ false, false };

		static bool confirm_is_ok{ false };
		static std::pair<bool, bool> copy_file_kinds{ init_pair_data };
		static bool first_time{ true };
		static std::vector<Path> file_names;
		static Path select_item;

		// 最初だけ
		if (first_time)
		{
			// ファイルパスに存在するファイルを走査し取得
			file_names = GetAllFileName(SaveFileDir);

			// グリットを保存している・開いているファイルを削除
			file_names.erase(std::remove_if(file_names.begin(), file_names.end(),
				[&](Path& path) { return (Grit::FileDir == path || open_file_path == path); }),
				file_names.end());

			// 拡張子を除いたファイル名を抽出する
			for (auto& name : file_names)
				name = name.stem();

			first_time = false;
		}

		// 「はい」を押した後、表示しない為
		if (!confirm_is_ok)
		{
			// 選択した後表示しない為
			if (copy_file_kinds == init_pair_data)
			{
				ImGui::Text(u8"コピーする種類を選んで下さい。");

				copy_file_kinds =
					ImguiTool::YesNoButton(u8"「開いているファイル」を「別のファイル」へコピー",
						u8"「別のファイル」を「開いているファイル」へコピー");
			}
			// 別のファイルへコピー
			else if (copy_file_kinds.first)
			{
				ImGui::Text(u8"%sから", open_file_path.stem().u8string().data());
				ImGui::SameLine();
				ImguiTool::SelectVectorString(u8"にコピーする", file_names, select_item);

				if (!editer_flg.is_save)
					ImGui::TextColored(IMGUI_RED,
						u8"保存されてませんよ？いいんっすか？");

				if (!select_item.empty())
				{
					ImGui::Text("AreYouOK?");

					if (ImGui::Button(u8"ハイ！"))
					{
						confirm_is_ok = true;
					}
				}
			}
			// 別のファイルからコピー
			else if (copy_file_kinds.second)
			{
				ImguiTool::SelectVectorString(u8"から", file_names, select_item);
				ImGui::SameLine();
				ImGui::Text(u8"%sへコピーする", open_file_path.stem().u8string().data());

				if (!editer_flg.is_save)
				{
					ImGui::TextColored(IMGUI_RED,
						u8"別のファイルからコピーするんで、このデータは消えまっすよ！");
					ImGui::TextColored(IMGUI_RED,
						u8"その後、読み込まれるんで現在のオブジェクトも完全に消えまっすよ!?");
				}

				if (!select_item.empty())
				{
					ImGui::Text("AreYouOK?");

					if (ImGui::Button(u8"ハイ！"))
					{
						confirm_is_ok = true;
					}
				}
			}

			ImguiTool::Spacing(5u);
			if (ImGui::Button(u8"キャンセル"))
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

			// 初回だけ
			if (first)
			{
				using std::filesystem::copy_file;

				constexpr auto Option{ std::filesystem::copy_options::overwrite_existing };

				other_file_path = (SaveFileDir / select_item) += FileExtension;

				// 別のファイルへコピー
				if (copy_file_kinds.first)
				{
					success_copy = copy_file(open_file_path, other_file_path, Option);
				}
				// 別のファイルからコピー
				else if (copy_file_kinds.second)
				{
					success_copy = copy_file(other_file_path, open_file_path, Option);

					Objects temp{ objects };  // 読み込み失敗した際に戻す為のデータ

					InitObjectClear();

					if (success_copy)
						success_copy = Input(other_file_path);
					else
						objects = std::move(temp);
				}

				first = false;
			}
			// 二回目以降
			else
			{
				// コピー成功
				if (success_copy)
				{
					static double timer;

					// 一定時間表示
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						// 別のファイルへコピー
						if (copy_file_kinds.first)
						{
							ImGui::Text(u8"「%s」へコピーしました。", other_file_path.u8string().data());
						}
						// 別のファイルからコピー
						else if (copy_file_kinds.second)
						{
							ImGui::Text(u8"「%s」からコピーしました。", other_file_path.u8string().data());
						}
					}
					else
					{
						// 初期化
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
				// コピー失敗
				else
				{
					static double timer;

					// 一定時間表示
					if ((timer += GMLIB->GetElapsedTime()) < PrintTimeMax)
					{
						ImGui::Text(u8"コピーに失敗しました。");
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
