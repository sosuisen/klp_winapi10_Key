#include <windows.h>
#include <string>
#include <format>
#include <chrono>

static const int WIN_WIDTH = 500;
static const int WIN_HEIGHT = 400;

// 問題のASCIIコード
static int iMon;
static std::wstring strMondai;

LRESULT CALLBACK WndProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	TCHAR szAppName[] = L"TypingApp";
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;

	// ウィンドウクラスの属性を設定
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	// ウィンドウクラスを登録
	if (!RegisterClass(&wc)) return 0;

	// ウィンドウを作成
	hwnd = CreateWindow(
		szAppName,
		L"タイピング練習(スペースで開始)",
		WS_OVERLAPPEDWINDOW,
		50, 50,
		WIN_WIDTH, WIN_HEIGHT,
		NULL, NULL,
		hInstance, NULL);

	if (!hwnd) return 0;

	// ウィンドウを表示
	ShowWindow(hwnd, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

int TypeStart(HWND hwnd)
{
	int n;

	n = rand() % 26; // 0から25までの整数値をランダムに生成
	// iMon = 97 + n;
	iMon = 65 + n; // A のASCIIコード65に0から25までの値を加えると、AからZまでのASCIIコードの数値になる
	// std::format を使うには、
	// プロジェクトのプロパティ「C++言語標準」を「ISO C++ 20 標準 (/std:c++20)」へ変更すること。
	strMondai = std::format(L"次の問題は　{:c}", iMon);

	InvalidateRect(hwnd, NULL, TRUE);
	return 0;
}

LRESULT CALLBACK WndProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;

	static std::wstring strInput;
	static std::wstring strCheck;
	static bool bStart = false, bSeikai = true;

	static std::chrono::system_clock::time_point start;

	switch (uMsg) {
	case WM_CREATE:
		srand((unsigned)time(NULL));    // 乱数の種を初期化する
		return 0;
	case WM_CHAR:
		// SPACE 0x20
		if (wParam == 0x20 && !bStart) {
			bStart = true;
			TypeStart(hwnd);

			// 反応時間測定開始
			start = std::chrono::system_clock::now();

			break;
		}

		if (bStart == false) break;
			
		// ESCAPE 0x1vb
		if (wParam == 0x1b) {
			strMondai = L"";
			strInput = L"";
			strCheck = L"";

			InvalidateRect(hwnd, NULL, true);
			bStart = false;
			break;
		}
		strInput = std::format(L"あなたの入力は　{:c}", (int)wParam);

		if (iMon == (int)wParam) {
			bSeikai = true;

			// 測定終了
			std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
			long msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


			strCheck = std::format(L"反応時間は　{:d}ミリ秒", msec);
			TypeStart(hwnd);
		}
		else {
			bSeikai = false;
			MessageBeep(MB_OK);
			strCheck = L"タイプミス！";
		}
		InvalidateRect(hwnd, NULL, TRUE);

		break;
	case WM_PAINT: {
		PAINTSTRUCT paint;
		hdc = BeginPaint(hwnd, &paint);

		TextOut(hdc, 0, 0, strMondai.c_str(), strMondai.length());

		TextOut(hdc, 0, 40, strInput.c_str(), strInput.length());
		if (bSeikai)
			SetTextColor(hdc, RGB(0, 0, 0));
		else
			SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, 0, 80, strCheck.c_str(), strCheck.length());

		EndPaint(hwnd, &paint);

		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}