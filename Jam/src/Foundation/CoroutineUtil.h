#pragma once
#include <Siv3D.hpp>
#include <coroutine>
#include <vector>

namespace Jam::Util
{
	// --- Task型（前方宣言が必要） ---
	struct Task {
		struct promise_type {
			Task get_return_object() {
				return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			void return_void() {}
			void unhandled_exception() { std::terminate(); }
		};

		using handle_type = std::coroutine_handle<promise_type>;
		handle_type coro_handle;

		Task(handle_type h) : coro_handle(h) {}
		Task(Task&& other) noexcept : coro_handle(other.coro_handle) {
			other.coro_handle = nullptr;
		}
		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		~Task() {
			// コルーチンの破棄はCoroutineUtilが管理するため、ここでは破棄しない
		}

		handle_type handle() const { return coro_handle; }
	};

	// --- コルーチン管理クラス ---
	struct CoroutineUtil
	{
		struct CoroutineEntry {
			std::coroutine_handle<> handle;
			double remaining;
		};

		static inline std::vector<CoroutineEntry> tasks;

		// coroutineを登録
		static void AddWait(std::coroutine_handle<> h, double seconds) {
			tasks.push_back({ h, seconds });
		}

		// Taskを登録して即実行
		static void Start(Task t) {
			auto h = t.handle();
			if (h && !h.done()) {
				AddWait(h, 0.0);
			}
		}

		// 毎フレーム呼ぶ
		static void Update(double delta) {
			// 時間を減算
			for (auto& t : tasks) {
				t.remaining -= delta;
			}

			// 時間切れのタスクを処理
			for (auto it = tasks.begin(); it != tasks.end();) {
				if (it->remaining <= 0) {
					auto h = it->handle;
					it = tasks.erase(it);  // 先にリストから削除

					if (h && !h.done()) {
						h.resume();

						// 完了していたら破棄
						if (h.done()) {
							h.destroy();
						}
					}
				}
				else {
					++it;
				}
			}
		}

		// すべてのタスクをクリア（シーン遷移時など）
		static void Clear() {
			for (auto& entry : tasks) {
				if (entry.handle && !entry.handle.done()) {
					entry.handle.destroy();
				}
			}
			tasks.clear();
		}
	};

	// --- co_awaitで使う待機構造体 ---
	struct WaitSeconds {
		double time;

		explicit WaitSeconds(double t) : time(t) {}

		bool await_ready() const noexcept { return time <= 0.0; }

		void await_suspend(std::coroutine_handle<> h) {
			CoroutineUtil::AddWait(h, time);
		}

		void await_resume() const noexcept {}
	};

	// --- フレーム待機 ---
	struct WaitForNextFrame {
		bool await_ready() const noexcept { return false; }

		void await_suspend(std::coroutine_handle<> h) {
			CoroutineUtil::AddWait(h, 0.0);
		}

		void await_resume() const noexcept {}
	};

	// --- 条件待機 ---
	template<typename Predicate>
	struct WaitUntil {
		Predicate pred;

		explicit WaitUntil(Predicate p) : pred(std::move(p)) {}

		bool await_ready() const { return pred(); }

		void await_suspend(std::coroutine_handle<> h) {
			CoroutineUtil::AddWait(h, 0.0);
		}

		bool await_resume() const { return pred(); }
	};
}
