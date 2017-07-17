#include <iostream>
#include <locale>
#include "../Code/MSXMLRead.hpp"
#include "Quiz.hpp"
#include "make_array.h"

static inline void ClearConsole() {
	std::system("cls");
}

static inline void PauseConsole() {
	std::system("pause");
}

static inline int Input() {
	std::string Buffer;
	std::cin >> Buffer;
	return std::stoi(Buffer);
}

int Input(const std::string Message, const int Max, const int Min) {
	try {
		if (Max < Min) return Input(Message, Min, Max);
		std::cout << Message << std::endl;
		const int InputNum = Input();
		if (InputNum < Min || Max < InputNum) throw std::exception("over input range");
		return InputNum;
	}
	catch (const std::exception& er) {
		std::cout << "Exception is occured." << std::endl << er.what() << std::endl;
		return Input(Message, Max, Min);
	}
}

int Input(const int Max, const int Min) {
	try {
		if (Max < Min) return Input(Min, Max);
		const int InputNum = Input();
		if (InputNum < Min || Max < InputNum) throw std::exception("over input range");
		return InputNum;
	}
	catch (const std::exception& er) {
		std::cout << "Exception is occured." << std::endl << er.what() << std::endl;
		return Input(Max, Min);
	}
}

static inline bool InputYesNo(const std::string Message) {
	return 1 == Input(Message, 2, 1);
}

static inline int InputYesNo() {
	return Input("�͂��c�P�@�������c�Q", 2, 1);
}

namespace PreludeQuestion {
	void Question1() {
		std::cout << "���O�m�F�P\n���Ȃ��̓p�e�B�V�G�܂��̓p�e�B�V�G�[���ł����H" << std::endl;
	}
	void Question2() {
		std::cout << "���O�m�F�Q\n���Ȃ��͖��F�p�e�B�V�G�[�����������Ă��̓��e�₻���ŏo�Ă����m����S�Ċo���Ă��܂����H" << std::endl;
	}
	int CalcGameLevel(std::pair<int, int> ChoosedAnswer) {
		/*���x��
		�͂��A�͂�		�F	1
		�͂��A������	�F	2
		�������A�͂�	�F	3
		�������A������	�F	4
		*/
		if (1 == ChoosedAnswer.first) ChoosedAnswer.first--;
		return ChoosedAnswer.first + ChoosedAnswer.second;
	}
	int ChooseQuestionLevel() {
		std::pair<int, int> ChoosedAnswer;
		for (bool LastAnswer = false; !LastAnswer;
			LastAnswer = [](const std::pair<int, int> ChoosedAnswer) {
			ClearConsole();
				auto YesNo = [](const int YesNoFlag) { return 1 == YesNoFlag ? "�͂�" : "������"; };
				std::cout << "����ł�낵���ł����H" << std::endl;
				Question1();
				std::cout << YesNo(ChoosedAnswer.first) << std::endl << std::endl;
				Question2();
				std::cout << YesNo(ChoosedAnswer.second) << std::endl << std::endl;
				return 1 == InputYesNo();
			}(ChoosedAnswer)
		) {
			ClearConsole();
			Question1();
			ChoosedAnswer.first = InputYesNo();
			ClearConsole();
			Question2();
			ChoosedAnswer.second = InputYesNo();
		}
		ClearConsole();
		std::cout << "�񓚂��肪�Ƃ��������܂��B����ł͎n�߂܂��B" << std::endl;
		PauseConsole();
		ClearConsole();
		return CalcGameLevel(ChoosedAnswer);
	}
}

std::vector<Quiz> LoadQuiz() {
	const auto CorrectAnswer = make_array(3, 2, 2, 1, 3, 3, 3, 3, 1, 1, 4, 2, 1, 6, 4, 2, 2, 3, 1);
	MSXMLRead QuizData(TEXT("question.xml"), TEXT("questionbook/questiondata/"));
	if (QuizData.CheckLength(TEXT("question")) == QuizData.CheckLength(TEXT("explanation"))
		&& QuizData.CheckLength(TEXT("question")) == QuizData.CheckLength(TEXT("IsNotForBiginner"))
		&& QuizData.CheckLength(TEXT("question")) == QuizData.CheckLength(TEXT("choices_num"))
		&& (long)CorrectAnswer.size() == QuizData.CheckLength(TEXT("question"))) throw std::runtime_error("���̓ǂݍ��݂̍ۂɃG���[���������܂����B");
	QuizData.Load<std::basic_string<TCHAR>>(TEXT("question"), TEXT("explanation"), TEXT("IsNotForBiginner"), TEXT("choices_num"));
	std::vector<Quiz> re;
	for (size_t i = 0; i < CorrectAnswer.size(); i++) {
		re.emplace_back(
			QuizData[TEXT("question")][i],
			QuizData[TEXT("explanation")][i],
			0 == std::stoi(QuizData[TEXT("IsNotForBiginner")][i]),
			std::stoi(QuizData[TEXT("choices_num")][i]),
			CorrectAnswer[i]
		);
	}
	return re;
}

auto DecideSequenceOfQuestionNumber(const int GameLevel) {
	static const auto sequence_of_questions_number_table = make_array(
		make_array<size_t>(0U, 1U, 2U, 3U, 4U, 8U, 12U, 16U, 17U, 18U),
		make_array<size_t>(0U, 1U, 2U, 3U, 5U, 9U, 13U, 16U, 17U, 18U),
		make_array<size_t>(0U, 1U, 2U, 3U, 6U, 10U, 14U, 16U, 17U, 18U),
		make_array<size_t>(0U, 1U, 2U, 3U, 7U, 11U, 15U, 16U, 17U, 18U)
	);
	return sequence_of_questions_number_table[GameLevel - 1];
}

bool Question(const Quiz quiz, std::vector<int> AnswerResultBuffer, const int GameLevel) {
	quiz.PrintQuestion();
	bool Continue = quiz.JudgeCorrect(Input(quiz.NumberOfChoices, 1));
	std::cout << (Continue ? "����" : "�s����") << "�ł��B" << std::endl;
	AnswerResultBuffer.emplace_back(Continue ? 1 : 0);
	if (!Continue && 4 == GameLevel) {
		quiz.PrintCorrectAnswer();
		quiz.PrintExplanation();
		if (quiz.IsNotForBeginner) Continue = true;
	}
	PauseConsole();
	ClearConsole();
	return Continue;
}

bool IsAllClear(const std::vector<int> AnswerResultBuffer) {
	int IsAllClear = 1;
	for (const auto& i : AnswerResultBuffer) IsAllClear &= i;
	return 1 == IsAllClear;
}

static void ShowLastMessage(const bool JudgeCorrect, const size_t IsAllClear, const int GameLevel) {
	static const auto message = make_array(
		make_array(
			"���̓~�X���V�т��ƐM���Ă��܂��B",
			"���̓~�X���V�т��ƐM���Ă��܂��B",
			"���p�e�B�ōĊm�F���Ă�����x���킵�Ă��������ˁB",
			"���͊撣���Ă��������ˁB"
		),
		make_array(
			"�ȒP�ł�����ˁH�ȒP�����Ă����܂���B",
			"�ȒP�ł�����ˁH�ȒP�����Ă����܂���B",
			"���Ȃ��͖{���ɖ��p�e�B�����Ă܂��ˁB",
			"�N���A���߂łƂ��������܂��B"
		)
	);
	std::cout << message[IsAllClear][GameLevel -1] << std::endl;
	if (JudgeCorrect && 4 == GameLevel && IsAllClear == 0) std::wcout << L"���͑S�␳����ڎw���܂��傤�B" << std::endl;
	PauseConsole();
	ClearConsole();
	std::cout
		<< "���̃Q�[���̖��͈ȏ�ɂȂ�܂��B��������ȃQ�[������Ăق����A���𑝂₵�Ăق���������܂�����Areadme�ɏ�����܂������[���A�h���X�ցB"
		<< std::endl
		<< "�V��Q�[����񓙂́Areadme�̃����N����A�N�Z�X�ł��܂��u���O�ɏ����L�ڂ��Ă����܂��B"
		<< std::endl;
	PauseConsole();
}

int main() {
	try {
#if defined(UNICODE)
		std::wcout.imbue(std::locale(""));
#endif
		std::cout << "�N�C�Y�@�`���Ȃ��̓p�e�B�V�G�`" << std::endl;
		if (!InputYesNo("�J�n�c�P�@�I���c�Q")) return 0;
		const std::vector<Quiz> QuizList = LoadQuiz();
		const int GameLevel = PreludeQuestion::ChooseQuestionLevel();
		const auto SequenceOfQuestionNumber = DecideSequenceOfQuestionNumber(GameLevel);
		bool JudgeCorrect = true;
		std::vector<int> ResultBuffer;
		for (size_t i = 0; JudgeCorrect && i < SequenceOfQuestionNumber.size(); i++) {
			JudgeCorrect = Question(QuizList[SequenceOfQuestionNumber[i]], ResultBuffer, GameLevel);
		}
		const bool AllClear = IsAllClear(ResultBuffer);
		ShowLastMessage(JudgeCorrect, AllClear ? 1U : 0U, GameLevel);
	}
	catch (const std::exception& er) {
		std::cout << std::endl << er.what() << std::endl;
	}
	return 0;
}
