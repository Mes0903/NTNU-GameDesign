#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "GameObject.hpp"
#include "Scene.hpp"
#include "include_5568ke.hpp"

inline enum class DialogChoice { non, A, B, C } dialogChoice;

enum class DialogType { DIALOG, QUIZ, GOODEND, BADEND };

struct DialogBase {
	DialogType type; // type info, to do reflection-like type check
	DialogBase(DialogType type) : type(type) {}
};

struct Dialog : DialogBase {
	std::vector<std::string> lines;
	Dialog() : DialogBase(DialogType::DIALOG) {}
};

struct Quiz : DialogBase {
	std::string question;
	std::vector<std::string> options;
	int ansIndex = -1;
	int userIndex = -1;
	int i_score{};
	std::vector<int> v_score;
	std::vector<std::string> feedback;
	Quiz() : DialogBase(DialogType::QUIZ) {}
};

struct NPC {
	std::shared_ptr<GameObject> go;
	std::vector<std::shared_ptr<DialogBase>> dialogs;

	// runtime state:
	bool routeEnabled{false};
	bool showIcon{false};
	bool inDialog{false};
	size_t scriptIndex{0};
	size_t lineIndex{0};
	
	// 新增：分數追蹤
	int totalScore{0};

	// Idle animation state - 新增
	bool isPlayingIdleAnimation{false};
	float idleAnimationTime{0.0f};
	int idleAnimationIndex{-1}; // 儲存idle動畫的索引
};

class DialogSystem {
public:
	static DialogSystem& getInstance();

	NPC& addNPC(std::shared_ptr<GameObject> go, std::vector<std::shared_ptr<DialogBase>> script)
	{
		npcs_.push_back({go, std::move(script)});
		// 初始化新增的NPC的idle動畫
		initializeNPCIdleAnimation(npcs_.back());

		return npcs_.back();
	}

	void update(Scene& scene, float dt);
	void render(Scene const& scene);
	void processInput(GLFWwindow* window);

private:
	DialogSystem() = default;

	static glm::vec2 worldToScreen(glm::vec3 const& pos, Scene const& scene, int viewportW, int viewportH);

	void renderDialogUI();
	void renderDialog(Dialog const& dialog, NPC& npc);
	void renderQuiz(Quiz const& quiz, NPC& npc);
	void renderEnding(Dialog const& ending, NPC& npc, bool isGoodEnding);

	void handleDialogProgress(NPC& npc);

	// 新增的idle動畫相關方法
	void initializeNPCIdleAnimation(NPC& npc);
	void updateNPCIdleAnimation(NPC& npc, float dt);
	void startIdleAnimation(NPC& npc);
	int findIdleAnimationIndex(std::shared_ptr<GameObject> const& go);

	std::vector<NPC> npcs_;
};

inline void initBegin(std::shared_ptr<GameObject> go)
{
	std::shared_ptr<Dialog> d1 = std::make_shared<Dialog>();
	d1->lines.push_back("（你原本想選修一門正常的課，走進分部圖書館後，看向手機確認選課系統時，發現自己莫名多了一門「遊戲程式與戀愛學特訓班」。）");
	d1->lines.push_back("（正準備點開查看時，突然被一隻有力的大手扯住袖子——）");
	d1->lines.push_back("老師：「割布麟同學，請問你母單嗎？」");
	d1->lines.push_back("割布麟：「......？」");
	d1->lines.push_back("老師：「我想...這門課應該很適合你，趕快進來吧！」");
	d1->lines.push_back("割布麟：「等、等等——這門課到底是什麼？！它根本不在課表裡啊！」");
	d1->lines.push_back("（老師神秘地推了推眼鏡，教室大門自動在割布麟身後關上。）");
	d1->lines.push_back("老師：「這是一門結合 AI、遊戲設計和……戀愛學的終極課程，你的任務很簡單——成功攻略我設計的 AI 角色，否則……直接不及格。」");
	d1->lines.push_back("割布麟：「什麼？！可是我母單20年！！！這太強人所難了吧」");
	d1->lines.push_back("老師（微笑）：「不會吧，連 NPC 都談不下來？」");
	d1->lines.push_back("（...）");

	std::shared_ptr<Quiz> q1 = std::make_shared<Quiz>();
	q1->question = "老師：「戀愛遊戲的核心是角色設計！來吧，為你的 AI 角色設計一個迷人的設定！」";
	q1->options.push_back("1.「應該有一個強烈的背景故事，讓角色有層次感！」");
	q1->options.push_back("2.「當然要有甜蜜的戀愛情節，製造心動瞬間！」");
	q1->options.push_back("3.「沉浸式互動才是王道，讓玩家自由選擇情節發展！」");

	std::shared_ptr<Dialog> d2 = std::make_shared<Dialog>();
	d2->lines.push_back("老師：「很好，現在，讓你的 AI 角色開始對話吧！」");
	d2->lines.push_back("（割布麟開始體驗第一場 AI 模擬對話，但……）");
	d2->lines.push_back("AI 角色：「初次見面……請輸入選項……」");
	d2->lines.push_back("（系統錯誤，AI 角色突然開始胡言亂語）");
	d2->lines.push_back("AI 角色：「這不是約會，而是統計數據的美妙運算！」");
	d2->lines.push_back("割布麟：「老師，這個 AI 真的能攻略嗎？！」");
	d2->lines.push_back("老師（推眼鏡）：「那就要看你的能力了。」");
	d2->lines.push_back("（進入決定初始好感度劇情，玩家選擇回應方式將決定分數）");

	std::shared_ptr<Quiz> q2 = std::make_shared<Quiz>();
	q2->question = "1. 心儀對象跟你說想出門看最近最流行的玫瑰園，你會穿什麼？";
	q2->options.push_back("A. 簡單的黑白灰格紋襯衫（0 分） ✅ 哥布林標準穿搭");
	q2->v_score.push_back(0);
	q2->options.push_back("B. 平常手臂有加強，穿高磅素T就好（10 分） ❌ 你是不是在炫手臂？這樣算裝備");
	q2->v_score.push_back(10);
	q2->options.push_back("C. GU 大地色穿搭，短褲白襪（5 分） ⚠️ 這已經有點時尚感，小心被判定為「有裝備」");
	q2->v_score.push_back(5);

	std::shared_ptr<Quiz> q3 = std::make_shared<Quiz>();
	q3->question = "2. 朋友揪去夜店玩，你的第一個反應是？";
	q3->options.push_back("A.「蛤？那邊不是很貴嗎？」（0 分） ✅ 哥布林不會亂課金");
	q3->v_score.push_back(0);
	q3->options.push_back("B.「誒剛好！可以揪認識的脆友在夜店見面」（10 分） ❌ 你這個心態…根本不是純種哥布林");
	q3->v_score.push_back(10);
	q3->options.push_back("C.「好啊。我常去。（結果回去偷偷焦慮襯衫會不會太正式。）」（5 分） ⚠️ 你居然在意穿搭，太有裝備感");
	q3->v_score.push_back(5);

	std::shared_ptr<Quiz> q4 = std::make_shared<Quiz>();
	q4->question = "3. 你正在用交友軟體，突然滑到一個超對你胃口的女生，你的開場白是？";
	q4->options.push_back("A.「我也喜歡這部電影！」（5 分） ⚠️ 雖然安全，但有點普通哥布林");
	q4->v_score.push_back(5);
	q4->options.push_back("B.「哈哈哈哈哈」（10 分） ❌ 這種開場方式，精靈級帥哥才用得起來");
	q4->v_score.push_back(10);
	q4->options.push_back("C.「嗨～尼看起來豪有氣質，平常喜歡看書嗎？」（0 分） ✅ 哥布林打安全牌，穩！");
	q4->v_score.push_back(0);

	std::shared_ptr<Quiz> q5 = std::make_shared<Quiz>();
	q5->question = "4. 你喜歡的女生說最近壓力好大，想要來點小確幸，你的選擇是？";
	q5->options.push_back("A.「記得你上次發限動想看夜景？今天晚上我開車載你去陽明山呀」（10 分） ❌ 這樣算「課金」，失去哥布林資格");
	q5->v_score.push_back(10);
	q5->options.push_back("B.「晚上送宵夜給你呀，你想吃什麼？」（0 分） ✅ 哥布林風格，只詢問卻不行動");
	q5->v_score.push_back(0);
	q5->options.push_back("C.「帶你去吃我家巷口的火鍋店！」（5 分） ⚠️ 這可能是小課金，但還能接受");
	q5->v_score.push_back(5);

	std::shared_ptr<Quiz> q6 = std::make_shared<Quiz>();
	q6->question = "5. 你長得如何？（誠實回答！）";
	q6->options.push_back("A.「長得普通啦，反正看順眼最重要。」（0 分） ✅ 哥布林標準自覺");
	q6->v_score.push_back(0);
	q6->options.push_back("B.「師大彭于晏」（10 分） ❌ 你是精靈，滾！");
	q6->v_score.push_back(10);
	q6->options.push_back("C.「還可以啦，有時候會被說耐看。」（5 分） ⚠️ 你可能在偽裝，觀察中");
	q6->v_score.push_back(5);

	std::shared_ptr<Quiz> q7 = std::make_shared<Quiz>();
	q7->question = "6. 女生問：「你 IG版面怎麼都沒發文？」你會怎麼回答？";
	q7->options.push_back("A.「懶得發，而且生活沒什麼特別的。」（0 分） ✅ 純正哥布林，沒社群戰力");
	q7->v_score.push_back(0);
	q7->options.push_back("B.「我都典藏了啦，沒什麼人在看。」（5 分） ⚠️ 有點小心機，哥布林應該完全不在意流量");
	q7->v_score.push_back(5);
	q7->options.push_back("C.「哈哈我都發摯友啦，等下加妳進去。」（10 分） ❌ 這樣太有手腕，哥布林沒這種社交技巧");
	q7->v_score.push_back(10);

	std::shared_ptr<Quiz> q8 = std::make_shared<Quiz>();
	q8->question = "7. 你的身高是？（誠實回答！）";
	q8->options.push_back("A.「178，剛好不超標！」（5 分） ⚠️你有自覺，還算哥布林");
	q8->v_score.push_back(5);
	q8->options.push_back("B.「182，不過應該還好吧？」（10 分） ❌ 抱歉，你是高級巨人族");
	q8->v_score.push_back(10);
	q8->options.push_back("C.「170，這題對我很友善。」（0 分） ✅ 你是地精，不是哥布林");
	q8->v_score.push_back(0);

	std::shared_ptr<Quiz> q9 = std::make_shared<Quiz>();
	q9->question = "8. 女生突然說：「你覺得男生應該主動付錢嗎？」你的反應？";
	q9->options.push_back("A.「AA 最公平吧？」（0 分） ✅ 純正哥布林哲學");
	q9->v_score.push_back(0);
	q9->options.push_back("B.「當然要付啊，小錢啦」（10 分） ❌ 這樣太課金");
	q9->v_score.push_back(10);
	q9->options.push_back("C.「要看關係啦，曖昧的話請一下也 OK 吧？」（5 分） ⚠️ 小心喔，這樣可能偏向半課金");
	q9->v_score.push_back(5);

	std::shared_ptr<Quiz> q10 = std::make_shared<Quiz>();
	q10->question = "9. 她要過生日，你會送什麼？";
	q10->options.push_back("A.「送手作的禮物比較有心意吧？」（0 分） ✅ 哥布林會省錢又有誠意");
	q10->v_score.push_back(0);
	q10->options.push_back("B.「送香水組合，之後再問她喜歡哪個味道」（10 分） ❌ 這樣算課金，出局！");
	q10->v_score.push_back(10);
	q10->options.push_back("C.「買個可愛的蛋糕小加手寫卡片。」（5 分） ⚠️ 這樣不錯，但有點小心機");
	q10->v_score.push_back(5);

	std::shared_ptr<Quiz> q11 = std::make_shared<Quiz>();
	q11->question = "10. 你有沒有女朋友？";
	q11->options.push_back("A.「沒有，之前追過但沒成功。」（0 分） ✅ 哥布林基本屬性");
	q11->v_score.push_back(0);
	q11->options.push_back("B.「有過幾個，但現在單身。」（10 分） ❌ 這樣你跳級了，不是哥布林");
	q11->v_score.push_back(10);
	q11->options.push_back("C.「剛被分手，但我還沒走出來。」（5 分） ⚠️ 這樣可以，但要觀察你是精靈還是哥布林");
	q11->v_score.push_back(5);

	NPC& npc = DialogSystem::getInstance().addNPC(go, {d1, q1, d2, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11});
	npc.routeEnabled = true;
	npc.go->invMass = 0;
}

inline void initA(std::shared_ptr<GameObject> go)
{
	std::shared_ptr<Dialog> d11 = std::make_shared<Dialog>();
	d11->lines.push_back("（場景：圖書館 801 教室，課堂開始，螢幕正播放「什麼是行為樹 AI」的簡報動畫）");
	d11->lines.push_back("周理安：「遊戲的核心是演算法與機制，而不是表面的情感渲染。」");
	d11->lines.push_back("老師：「很好，現在我們來設計 NPC 的行為樹，讓角色能根據玩家的選擇產生不同的對話與反應。」");
	d11->lines.push_back("主角（murmur）：「這門課的內容……怎麼越來越像演算法的學習課程了？」");
	d11->lines.push_back("老師：「要攻略 AI，首先你得思考：如果 NPC 有思考能力，它會根據什麼改變行為？」");
	d11->lines.push_back("主角：「這比攻略活人還難吧……」");
	d11->lines.push_back("周理安（推了推眼鏡）：「思考要條理、邏輯要清晰——不然連 ‘if’ 條件都判斷不了。」");

	std::shared_ptr<Dialog> d12 = std::make_shared<Dialog>();
	d12->lines.push_back("教學 1：什麼是行為樹？");
	d12->lines.push_back("教室裡，老師拿出一塊寫滿愛心與箭頭的白板。");
	d12->lines.push_back("老師：「戀愛不是亂槍打鳥，是有策略的行為流程。行為樹就是一種用來安排行為順序的結構——像是戀愛流程圖！」");
	d12->lines.push_back("周理安：「根節點就是起點，從這裡開始分析你的戀愛流程。」");
	d12->lines.push_back("【補充】");
	d12->lines.push_back("行為樹從「根節點」開始");
	d12->lines.push_back("下方是「控制節點」與「行為節點」");
	d12->lines.push_back("執行會從上往下，一步步判斷");

	std::shared_ptr<Quiz> q11 = std::make_shared<Quiz>();
	q11->question = "測驗 1：你第一次傳訊息給喜歡的人時，哪個最像「行為樹的根節點」？";
	q11->options.push_back("A. 說晚安");
	q11->options.push_back("B. 確認對方有沒有上線");
	q11->options.push_back("C. 決定要不要傳訊息");
	q11->options.push_back("D. 看對方的限時動態");
	q11->v_score = {5, 5, 0, 5}; // C是正確答案，哥布林風格給0分
	q11->feedback.push_back("✅ 正解：C");
	q11->feedback.push_back("💬 主角：「原來戀愛也有 if-else 條件判斷啊……」");
	q11->ansIndex = 2;

	std::shared_ptr<Quiz> q12 = std::make_shared<Quiz>();
	q12->question = "測驗 2：哪個說法最接近行為樹的「從上往下、從左到右執行」？";
	q12->options.push_back("A. 先看對方限動再決定行動");
	q12->options.push_back("B. 同時去對方家門口、教室門口、IG留言");
	q12->options.push_back("C. 先告白再看對方長怎樣");
	q12->options.push_back("D. 隨便點一個選項看運氣");
	q12->v_score = {0, 10, 10, 5}; // A是正確答案，哥布林風格給0分
	q12->feedback.push_back("✅ 正解：A");
	q12->feedback.push_back("💬 周理安：「邏輯比衝動重要。這是基本。」");
	q12->ansIndex = 0;

	std::shared_ptr<Dialog> d21 = std::make_shared<Dialog>();
	d21->lines.push_back("教學 2：Selector & Sequence");
	d21->lines.push_back("走廊上，理安遞給你一張便條紙：「這是戀愛流程的兩種邏輯模型。看懂再說話。」");
	d21->lines.push_back("Selector（選擇）：像是「今天邀約的方式」");
	d21->lines.push_back("如果約喝咖啡失敗 → 試約吃拉麵 → 試約看書");
	d21->lines.push_back("成功就停止，像是在試方法");
	d21->lines.push_back("Sequence（序列）：像是「告白前的準備流程」");
	d21->lines.push_back("要確保：對方心情好 ✔️、自己沒口臭 ✔️、場地氣氛 OK ✔️ → 才能執行「告白」");

	std::shared_ptr<Quiz> q21 = std::make_shared<Quiz>();
	q21->question = "測驗 3：你要跟理安告白，哪個是 Sequence 的例子？";
	q21->options.push_back("A. 直接告白失敗了就跑走");
	q21->options.push_back("B. 確認她在、準備花、深呼吸、才走過去");
	q21->options.push_back("C. 同時拿三束花丟給三個人看誰接");
	q21->options.push_back("D. 靠直覺衝過去喊「我喜歡你」");
	q21->v_score = {0, 10, 5, 0}; // A和D是哥布林風格，B是正確但太有計劃
	q21->feedback.push_back("✅ 正解：B");
	q21->feedback.push_back("💬 主角：「感覺像在寫 SOP……戀愛還真嚴謹」");
	q21->ansIndex = 1;

	std::shared_ptr<Quiz> q22 = std::make_shared<Quiz>();
	q22->question = "測驗 4：Selector 比喻成戀愛狀況，最接近哪個？";
	q22->options.push_back("A. 告白一定要成功，不然整個流程停止");
	q22->options.push_back("B. 今天一定要約成，不管用什麼方法");
	q22->options.push_back("C. 失敗一次就放棄");
	q22->options.push_back("D. 每個條件都要達成才能告白");
	q22->v_score = {5, 10, 0, 5}; // C是哥布林放棄風格，B是正確但太積極
	q22->feedback.push_back("✅ 正解：B");
	q22->feedback.push_back("💬 周理安：「會變通的人，戀愛才有機會。」");
	q22->ansIndex = 1;

	std::shared_ptr<Dialog> d31 = std::make_shared<Dialog>();
	d31->lines.push_back("教學 3：Action（行為節點）");
	d31->lines.push_back("你終於鼓起勇氣問理安：「那角色實際上怎麼做事情？」她翻開一本筆記。");
	d31->lines.push_back("理安：「葉子節點就是具體動作，比如走向某人、打招呼、送花。這些動作才會真的發生在遊戲中。」");
	d31->lines.push_back("【補充】");
	d31->lines.push_back("Action 節點是行為樹的末端");
	d31->lines.push_back("控制節點只是「流程管控」，Action 才是「真的執行」");

	std::shared_ptr<Quiz> q31 = std::make_shared<Quiz>();
	q31->question = "測驗 5：下列哪一個最像是 Action 節點？";
	q31->options.push_back("A. 思考是否要送花");
	q31->options.push_back("B. 規劃今天的行程");
	q31->options.push_back("C. 真正遞出那一束花");
	q31->options.push_back("D. 猶豫要不要傳訊息");
	q31->v_score = {5, 5, 10, 0}; // D是哥布林猶豫風格，C是正確但需要勇氣
	q31->feedback.push_back("✅ 正解：C");
	q31->feedback.push_back("💬 主角：「光想不行，還是得遞出花的那一刻才是真正的行動！」");
	q31->ansIndex = 2;

	std::shared_ptr<Quiz> q32 = std::make_shared<Quiz>();
	q32->question = "測驗 6：你設計一個 NPC，當他看到喜歡的人時會「笑」這個行為，這是什麼？";
	q32->options.push_back("A. 控制節點");
	q32->options.push_back("B. Sequence");
	q32->options.push_back("C. Action 節點");
	q32->options.push_back("D. 根節點");
	q32->v_score = {5, 5, 10, 5}; // C是正確答案但需要理解
	q32->feedback.push_back("✅ 正解：C");
	q32->feedback.push_back("💬 周理安：「角色不笑，你就沒有機會了。」");
	q32->ansIndex = 2;

	std::shared_ptr<Dialog> d41 = std::make_shared<Dialog>();
	d41->lines.push_back("教學 4：成功與失敗（Success/Failure）");
	d41->lines.push_back("【劇情】");
	d41->lines.push_back("你問理安：「如果我遞花她沒接呢？」");
	d41->lines.push_back("她淡淡說：「那就是失敗。行為樹每一步都會回報『成功』或『失敗』，這會影響整體流程能不能繼續下去。」");
	d41->lines.push_back("【補充】");
	d41->lines.push_back("行為節點會回傳「Success」或「Failure」");
	d41->lines.push_back("控制節點根據回傳值決定是否繼續下一步");

	std::shared_ptr<Quiz> q41 = std::make_shared<Quiz>();
	q41->question = "測驗 7：你試圖讓 NPC 說「我喜歡你」，但對方角色不在現場。這個行為的回傳是？";
	q41->options.push_back("A. Success");
	q41->options.push_back("B. Failure");
	q41->options.push_back("C. Running");
	q41->options.push_back("D. Happy");
	q41->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林不懂技術
	q41->feedback.push_back("✅ 正解：B");
	q41->feedback.push_back("💬 主角：「所以……這段戀愛判定失敗 Q_Q」");
	q41->ansIndex = 1;

	std::shared_ptr<Quiz> q42 = std::make_shared<Quiz>();
	q42->question = "測驗 8：在 Sequence 中，第二步驟失敗了，後面的行為還會執行嗎？";
	q42->options.push_back("A. 一定會");
	q42->options.push_back("B. 不會");
	q42->options.push_back("C. 會視心情決定");
	q42->options.push_back("D. 看遊戲設定");
	q42->v_score = {5, 10, 0, 5}; // B是正確答案，C是哥布林隨心所欲
	q42->feedback.push_back("✅ 正解：B");
	q42->feedback.push_back("💬 理安：「戀愛流程中出現漏洞，當然得中止重來。」");
	q42->ansIndex = 1;

	std::shared_ptr<Dialog> d51 = std::make_shared<Dialog>();
	d51->lines.push_back("教學 5：Running（執行中狀態）");
	d51->lines.push_back("某天下課後，你試著模擬一段 NPC 和玩家互動的劇情給理安看。她點頭：「你少了一個關鍵狀態：Running。」");
	d51->lines.push_back("【補充】");
	d51->lines.push_back("有些行為不是立即成功或失敗，而是正在進行中（如等待回覆）");
	d51->lines.push_back("這種狀態叫做 Running");

	std::shared_ptr<Quiz> q51 = std::make_shared<Quiz>();
	q51->question = "測驗 9：你傳訊息後，對方已讀但還沒回，這是哪種狀態？";
	q51->options.push_back("A. Success");
	q51->options.push_back("B. Failure");
	q51->options.push_back("C. Running");
	q51->options.push_back("D. Timeout");
	q51->v_score = {5, 0, 10, 5}; // C是正確答案，B是哥布林悲觀想法
	q51->feedback.push_back("✅ 正解：C");
	q51->feedback.push_back("💬 主角：「這才是真正最折磨人的狀態……戀愛中的 loading 畫面。」");
	q51->ansIndex = 2;

	std::shared_ptr<Quiz> q52 = std::make_shared<Quiz>();
	q52->question = "測驗 10：NPC 開始走向喜歡的人，中途還沒走到，屬於什麼狀態？";
	q52->options.push_back("A. Failure");
	q52->options.push_back("B. Waiting");
	q52->options.push_back("C. Running");
	q52->options.push_back("D. Ending");
	q52->v_score = {0, 5, 10, 5}; // C是正確答案，A是哥布林悲觀
	q52->feedback.push_back("✅ 正解：C");
	q52->feedback.push_back("💬 周理安：「在愛情裡，進行中的動作，也是一種希望。」");
	q52->ansIndex = 2;

	std::shared_ptr<Dialog> e1 = std::make_shared<Dialog>();
	e1->type = DialogType::GOODEND;
	e1->lines.push_back("✅ 成功攻略周理安");
	e1->lines.push_back("夕陽下，你與理安一起站在天台邊緣，風輕輕吹起她的頭髮。");
	e1->lines.push_back("周理安（低聲）：「你居然……真的學會了全部的行為樹邏輯？就連 Running 的邏輯都能用來比喻等喜歡的人回訊息……」");
	e1->lines.push_back("你苦笑：「我為了能和妳說上話，特訓了好幾天。」");
	e1->lines.push_back("她輕輕瞪了你一眼，然後眼神轉為柔和。");
	e1->lines.push_back("周理安：「那我現在的狀態是什麼？」");
	e1->lines.push_back("你盯著她的眼睛：「應該是……Running，因為我還不知道你對我的回應。」");
	e1->lines.push_back("周理安（停頓）：「錯了，是 Success，你這笨蛋。」");
	e1->lines.push_back("「你成功通關了《遊戲程式與戀愛學特訓班》：周理安路線｜攻略達成」");

	std::shared_ptr<Dialog> e2 = std::make_shared<Dialog>();
	e2->type = DialogType::BADEND;
	e2->lines.push_back("❌ 攻略失敗");
	e2->lines.push_back("【場景：空蕩蕩的教室，結課的最後一晚】");
	e2->lines.push_back("你坐在位子上，看著空空如也的白板。桌上放著你的測驗結果——答錯了太多題。");
	e2->lines.push_back("老師（拍你肩膀）：「不錯了，至少你撐到最後。不過這堂課不是誰都能順利通關的。」");
	e2->lines.push_back("你低頭一笑，望向窗外。");
	e2->lines.push_back("主角 murmur：「原來……就算懂了一堆理論，戀愛還是不能全靠演算法。」");
	e2->lines.push_back("這時，門口傳來熟悉的腳步聲。");
	e2->lines.push_back("周理安：「……你不及格了耶。」");
	e2->lines.push_back("你尷尬點頭：「對啊，我猜我沒辦法用行為樹攻略你了。」");
	e2->lines.push_back("她站在門邊，忽然露出一點笑意。");
	e2->lines.push_back("周理安：「那就……改用別的演算法再試一次啊。」");
	e2->lines.push_back("（畫面轉黑，顯示文字）");
	e2->lines.push_back("「你未能通關《遊戲程式與戀愛學特訓班》：周理安路線｜未攻略成功，但故事還沒結束……？」");

	NPC& npc = DialogSystem::getInstance().addNPC(go, {d11, d12, q11, q12, d21, q21, q22, d31, q31, q32, d41, q41, q42, d51, q51, q52, e1, e2});
	npc.go->invMass = 0;
}

inline void initB(std::shared_ptr<GameObject> go)
{
	std::shared_ptr<Dialog> d11 = std::make_shared<Dialog>();
	d11->lines.push_back("主角剛剛坐下，就被一疊粉紅色的劇本砸到。");
	d11->lines.push_back("林夢瑤（驚呼）：「啊！對不起對不起！我剛剛想測試拋物線軌跡的感覺，沒想到砸到人了！」");
	d11->lines.push_back("主角：「拋物線……劇本……？這門課到底是來上程式的還是來拍偶像劇的？」");
	d11->lines.push_back("此時，劉焱成老師大聲宣布：");
	d11->lines.push_back("老師：「今天開始，我們將進入《戀愛路線模擬與情感選擇架構》模組。你們要做的，就是設計一款讓玩家心跳加速、愛到卡慘死的遊戲。」");
	d11->lines.push_back("林夢瑤（雙眼閃亮）：「這不就是我一直夢想的那種、會讓人忘記現實的戀愛世界嗎？」");
	d11->lines.push_back("你猶豫了片刻，卻又無法抗拒她的熱情邀請，一起踏上這條粉紅泡泡的學習路線……");

	std::shared_ptr<Dialog> d12 = std::make_shared<Dialog>();
	d12->lines.push_back("教學1：戀愛劇本的心跳公式——情節張力與起承轉合");
	d12->lines.push_back("【場景】 圖書館801教室，一張堆滿粉紅便條紙的白板上，寫滿了各種劇情模板。林夢瑤手拿馬克筆，正熱血沸騰地畫出愛心箭頭與戀愛三角。");
	d12->lines.push_back(
			"林夢瑤：「一個讓人上癮的戀愛劇情，不能只是男主遞衛生紙給女主就感天動地好嗎～要有『情緒張力』！有衝突、有誤會、有心動才有價值！這些都要安"
			"排在劇本的起承轉合裡！」");
	d12->lines.push_back("主角（murmur）：「這堂課是戀愛心理還是結構寫作⋯⋯？」");
	d12->lines.push_back(
			"林夢瑤：「不！這是心跳設計學！『起』要有獨特邂逅；『承』要鋪陳日常互動；『轉』得來個衝突或誤會；『合』則是高潮與情感昇華。太平凡的戀愛，"
			"只會讓玩家點右上角退出。」");
	d12->lines.push_back("（螢幕出現小抄圖解：「戀愛劇本四階段」）");

	std::shared_ptr<Quiz> q11 = std::make_shared<Quiz>();
	q11->question = "Q1：哪一個事件最適合安排在『轉』的階段？";
	q11->options.push_back("A. 男主遞早餐給女主");
	q11->options.push_back("B. 女主誤會男主和青梅竹馬交往");
	q11->options.push_back("C. 男女主角在圖書館第一次相遇");
	q11->options.push_back("D. 兩人互許心願去看流星雨");
	q11->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林浪漫但不切實際
	q11->feedback.push_back("正解：B");
	q11->feedback.push_back("💬林夢瑤：「對啦對啦！『轉』就是要讓玩家一邊生氣一邊還想繼續玩，這種誤會才是戀愛遊戲的靈魂啊～」");

	std::shared_ptr<Quiz> q12 = std::make_shared<Quiz>();
	q12->question = "Q2：以下哪個情節最適合當作『起』的開端？";
	q12->options.push_back("A. 男女主角在社團吵架");
	q12->options.push_back("B. 男主為女主擋下掉落的書本");
	q12->options.push_back("C. 女主看到男主和別人牽手");
	q12->options.push_back("D. 女主悄悄觀察男主的社群帳號很久");
	q12->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林偷窺風格
	q12->feedback.push_back("正解：B");
	q12->feedback.push_back("💬林夢瑤：「這就是所謂的『瞬間命運感』！一個經典又百看不厭的邂逅方式，沒問題吧！」");

	std::shared_ptr<Dialog> d21 = std::make_shared<Dialog>();
	d21->lines.push_back("教學 2：角色性格建構：從 MBTI 到反差萌");
	d21->lines.push_back(
			"隔天清晨，圖書館窗邊灑進一縷光，林夢瑤蹲在角落，一邊看著偶像劇的設定本，一邊激動地在筆記本上畫著人設表。主角靠近時，她突然轉頭，眼神閃爍。");
	d21->lines.push_back("林夢瑤：「我昨天夢到一個超帥的反社會型男主，他外表冷酷，實際會偷偷幫女主撿掉在地上的補習班傳單……這種『反差』你懂嗎！超級重要的啦！」");
	d21->lines.push_back("主角（murmur）：「反社會型……撿傳單？這是什麼神奇組合……？」");
	d21->lines.push_back(
			"林夢瑤：「來，我們先不講夢了，講理論！角色要有邏輯，但不能無聊！MBTI可以幫你建立角色骨架，但真正讓人愛上的，是那個出其不意的『反差』——比"
			"如冷面殺手也會怕蟑螂！」");
	d21->lines.push_back("她突然舉起一個白板，上面畫了三個角色設定，並用愛心和爆炸圖標標註：");
	d21->lines.push_back("INFJ：沉默寡言 → 寫日記會寫十頁戀愛妄想");
	d21->lines.push_back("ENTP：話多又跳Tone → 私底下會做超細緻的便當");
	d21->lines.push_back("ISTP：冷靜理性 → 對戀愛毫無經驗，只會模仿電影告白橋段");
	d21->lines.push_back("林夢瑤：「看出來了嗎？不是MBTI定一切，而是你怎麼在既有框架裡製造驚喜！這樣角色才會有人氣嘛～」");

	std::shared_ptr<Quiz> q21 = std::make_shared<Quiz>();
	q21->question = "Q3：以下哪一個是常見的「反差萌」設定？";
	q21->options.push_back("A. 女主是溫柔體貼型，但其實擅長格鬥");
	q21->options.push_back("B. 男主是活潑型，經常搞笑又遲到");
	q21->options.push_back("C. 女主是害羞型，會迴避所有互動");
	q21->options.push_back("D. 男主是學霸，對感情毫無興趣");
	q21->v_score = {10, 5, 0, 5}; // A是正確答案，C是哥布林害羞風格
	q21->feedback.push_back("正解：A");
	q21->feedback.push_back("💬林夢瑤：「沒錯沒錯～反差就是你原本以為她只能溫柔，結果她一拳打飛流氓，這才叫讓人心動嘛！」");

	std::shared_ptr<Quiz> q22 = std::make_shared<Quiz>();
	q22->question = "Q4：哪個角色設定最有機會吸引喜歡「理性男」的玩家？";
	q22->options.push_back("A. INFP，時常情緒波動，夢想成為詩人");
	q22->options.push_back("B. ESTJ，重視效率，會依照時間表談戀愛");
	q22->options.push_back("C. ISFP，喜歡自己一個人待在樹下發呆");
	q22->options.push_back("D. ENFP，每天都有新的戀愛理論想分享");
	q22->v_score = {5, 10, 0, 5}; // B是正確答案，C是哥布林獨處風格
	q22->feedback.push_back("正解：B");
	q22->feedback.push_back("💬林夢瑤：「對啊！有些玩家就是吃這套『規則系戀愛』，而且越硬派越有反差潛力，比如他搞不好還會做愛情Excel表格呢！」");

	std::shared_ptr<Dialog> d31 = std::make_shared<Dialog>();
	d31->lines.push_back("教學 3：情緒是糖，節奏是鹽");
	d31->lines.push_back(
			"你與林夢瑤坐在圖書館八樓窗邊，外頭雨滴滴答敲著玻璃，她正翻閱一本標題是《讓你的主角哭得觀眾痛快》的戀愛劇本寫作書。你們要一起學習：如何設"
			"計角色的情緒曲線與劇情節奏。");
	d31->lines.push_back(
			"林夢瑤（雙眼閃亮）：「欸欸，你有發現嗎？所有讓人超級上頭的戀愛劇情——都會有那種『突然好甜！然後下一秒就虐爆』的轉折！你不覺得超帶感嗎？」");
	d31->lines.push_back("主角（你）：「呃……帶感是什麼單位？」");
	d31->lines.push_back("林夢瑤：「拜託，戀愛的節奏就是要像糖鹽混著吃！不能一直甜，也不能一直虐——你要讓觀眾‘以為要親了結果掀桌’，才會尖叫啊～」");
	d31->lines.push_back("（林夢瑤遞給你一本自製筆記，封面還畫了可愛的愛心爆炸圖）");
	d31->lines.push_back("📘 情緒曲線的基本原則：");
	d31->lines.push_back("角色必須經歷變化（不能從頭到尾都一樣開心/傷心）");
	d31->lines.push_back("故事節奏要有張力（要有「推進-衝突-釋放」的節拍）");
	d31->lines.push_back("給觀眾心理落差，才能產生情感參與");
	d31->lines.push_back("📘 常見的戀愛節奏安排範例：");
	d31->lines.push_back("誤會 → 傷心 → 再相遇 → 心動 → 誤會再升級 → 大告白 → Happy End");
	d31->lines.push_back("或是：突然親上去 → 被打 → 發現對方其實是間諜 → 邊逃亡邊談戀愛");
	d31->lines.push_back("林夢瑤：「總之你只要記住：讓角色痛，觀眾就會愛！（停頓）啊不對，是讓角色有情緒曲線，觀眾才會投入啦嘿嘿～」");

	std::shared_ptr<Quiz> q31 = std::make_shared<Quiz>();
	q31->question = "Q5. 以下哪一組情緒曲線更容易讓玩家投入？";
	q31->options.push_back("A. 主角從頭到尾都很開心，一路跟女主角打情罵俏，最後順利交往。");
	q31->options.push_back("B. 主角先討厭女主角→共患難→慢慢理解對方→產生情愫→突發衝突→最後和好。");
	q31->options.push_back("C. 主角一出場就大告白，然後開始甜蜜膩死人的生活。");
	q31->options.push_back("D. 主角一直傷心，女主角也沒出現，最後兩人都沒交集。");
	q31->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林悲觀結局
	q31->feedback.push_back("✅ 正解：B");
	q31->feedback.push_back("林夢瑤：沒錯～因為它包含了「角色成長、情緒起伏、情節反轉」，是最符合情緒曲線設計原則的節奏！");

	std::shared_ptr<Quiz> q32 = std::make_shared<Quiz>();
	q32->question = "Q6. 如果你要讓觀眾在第六集開始瘋狂嗑糖，你應該在前幾集怎麼安排劇情？";
	q32->options.push_back("A. 前五集完全沒互動，第六集直接接吻。");
	q32->options.push_back("B. 前幾集先鋪梗、互動冷淡、第六集突然有破防小舉動（例如意外牽手）。");
	q32->options.push_back("C. 前五集瘋狂灑糖，第六集也繼續曬恩愛。");
	q32->options.push_back("D. 第一集就親、第六集換另一個人親。");
	q32->v_score = {0, 10, 5, 5}; // B是正確答案，A是哥布林無互動風格
	q32->feedback.push_back("✅ 正解：B");
	q32->feedback.push_back("林夢瑤：是的！要讓觀眾感受到「進展」，就要先鋪墊反差，才會在第六集被甜到尖叫！");

	std::shared_ptr<Dialog> d41 = std::make_shared<Dialog>();
	d41->lines.push_back("教學 4：角色語言風格與台詞設計");
	d41->lines.push_back("（圖書館 801教室中，林夢瑤正在拿出一本厚到可以當枕頭的《戀愛遊戲名場面語錄解析》。她一臉興奮地拍了拍你的肩膀）");
	d41->lines.push_back("林夢瑤：「你知道嗎？一個角色的靈魂，其實是藏在她說話的方式裡！講話沒特色，就像告白只說『我喜歡你』——會被當成詐騙訊息！」");
	d41->lines.push_back("（老師從遠方走來，突然一手抽出一張「戀愛語氣診斷表」，像魔法少女變身那樣撒出星光紙片）");
	d41->lines.push_back("劉焱成老師：「設計語言風格，就是設計一種人格的濾鏡！要讓每個角色講話時，玩家能用耳朵分辨出他們的靈魂濃度！」");
	d41->lines.push_back("主角（murmur）：「我昨天夢到自己講話講到被戀愛選項淹沒，最後只能靠一根吸管呼吸……」");
	d41->lines.push_back(
			"林夢瑤忘我地興奮接話：「那我們來設計角色的語言人格——有的溫柔、有的傲嬌、有的裝酷、有的講話像機器人，讓每句台詞都能成為玩家截圖的動機！」");
	d41->lines.push_back("【教學知識重點】");
	d41->lines.push_back("語言風格定義：根據角色背景、個性，設計符合角色語調的語句（例如：高冷、撒嬌、直接、繞圈子）。");
	d41->lines.push_back("角色台詞設計技巧：一句話內盡量包含角色情緒與獨特表達方式。例如撒嬌型角色：「你不理我，我就……我就不開遊戲了啦 QAQ」");
	d41->lines.push_back("戀愛遊戲常見風格範例：");
	d41->lines.push_back("溫柔型：「如果你願意的話……可以陪我一下嗎？」");
	d41->lines.push_back("傲嬌型：「才、才不是特地來找你的呢！」");
	d41->lines.push_back("搞怪型：「吃下這顆糖你就得娶我，這是……呃，糖果契約？」");
	d41->lines.push_back("機械風型：「資料分析中。感情異常感知提升27%。建議：心動。」");

	std::shared_ptr<Quiz> q41 = std::make_shared<Quiz>();
	q41->question = "Q7. 傲嬌型女主角要對男主角告白，但她無法坦率表達。請選出最符合傲嬌語氣的告白句：";
	q41->options.push_back("A.「我一直很喜歡你，從第一天就知道了。」");
	q41->options.push_back("B.「才、才不是特別想每天看到你啦，只是你太礙眼了啦笨蛋……」");
	q41->options.push_back("C.「你這樣讓我感覺很特別，我想和你試著交往看看。」");
	q41->options.push_back("D.「你今天還是這麼溫柔，像春天一樣。」");
	q41->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林直白但不合題意
	q41->feedback.push_back("✅ 正確答案：B");
	q41->feedback.push_back("📘 解析：傲嬌類型角色的特徵是言不由衷、嘴硬心軟，會用「否定語」包裝真實情感，例如「才不是……」「誰管你啊……」");

	std::shared_ptr<Quiz> q42 = std::make_shared<Quiz>();
	q42->question = "Q8. 你正在設計一個機器人女友角色，她會根據玩家互動變化語調。請選出最能代表她風格的句子：";
	q42->options.push_back("A.「嘿，你又遲到了，我可是一直在等你。」");
	q42->options.push_back("B.「正在辨識……心率異常上升。資料庫標記為『喜歡』。」");
	q42->options.push_back("C.「你來啦～今天也要努力戀愛唷！」");
	q42->options.push_back("D.「不、不、不行這樣啦……我會害羞的 >///<」");
	q42->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林害羞風格
	q42->feedback.push_back("✅ 正確答案：B");
	q42->feedback.push_back("📘 解析：機器人型角色常融合理性邏輯與感情異常感知，語句中會出現「分析」「資料庫」「異常數值」等冷靜詞語，但隱含溫度。");

	std::shared_ptr<Dialog> d51 = std::make_shared<Dialog>();
	d51->lines.push_back("教學 5：：結局分歧與玩家影響設計");
	d51->lines.push_back("【場景】");
	d51->lines.push_back(
			"夜深了，801教室只剩主角與林夢瑤兩人。窗外是靜謐的校園，螢光燈偶爾閃爍。林夢瑤一邊喝著便利商店買來的熱可可，一邊攤開她的戀愛遊戲企劃書。");
	d51->lines.push_back("林夢瑤：「戀愛遊戲最迷人的地方，不只是誰跟誰在一起…而是『怎麼走到這裡的』。」");
	d51->lines.push_back("主角：「所以……玩家做的每個選擇，會導致不同的結局？」");
	d51->lines.push_back("林夢瑤：「對，這就像是人生分支模擬器，選擇愈多、愈能讓玩家感受到『我影響了這段感情』的重量。」");
	d51->lines.push_back("老師突然從黑板後面探出頭，像幽靈一樣冷不防地插話：");
	d51->lines.push_back(
			"劉焱成老師：「結局設計分三種：感情成功 or 失敗，角色改變與否，玩家的自我感受。設計者要問自己一個問題——『結束時，角色和玩家都得到了什麼？』」");
	d51->lines.push_back("林夢瑤：「簡單講，就是讓『在一起』這件事，不是按鈕，而是一段旅程的證明。」");
	d51->lines.push_back("【教學知識重點】");
	d51->lines.push_back("結局類型：");
	d51->lines.push_back("True Ending：角色與玩家走到深度共鳴（例如戀愛與成長兼得）");
	d51->lines.push_back("Bad Ending：因選擇錯誤，感情破裂或角色黑化");
	d51->lines.push_back("Neutral Ending：彼此尊重離開，保有好感但不交往");
	d51->lines.push_back("影響設計方法：");
	d51->lines.push_back("用「好感度」或「關鍵選擇」控制分歧");
	d51->lines.push_back("某些對話選項會累積分數，影響角色信任值");
	d51->lines.push_back("可設計隱藏選項與特定條件才解鎖的True Ending");
	d51->lines.push_back("情感回饋強化技巧：");
	d51->lines.push_back("結尾台詞設計：用一句讓人記住的話，總結感情旅程");
	d51->lines.push_back("簡單畫面演出（煙火、牽手、角色消失等）加深記憶");

	std::shared_ptr<Quiz> q51 = std::make_shared<Quiz>();
	q51->question = "題目 9. 情境：你要設計一個結局，讓玩家因錯過所有重要選擇導致 Bad Ending，請問哪個選項最能符合「戀愛失敗但角色成長」的路線？";
	q51->options.push_back("A. 角色決定轉學，兩人約定下次重逢時再重新開始");
	q51->options.push_back("B. 玩家在結尾時向角色告白成功，甜蜜牽手");
	q51->options.push_back("C. 角色消失在遊戲中，玩家找不到任何結局資訊");
	q51->options.push_back("D. 角色對玩家表示失望，並選擇離開，畫面漸黑");
	q51->v_score = {10, 5, 0, 5}; // A是正確答案，C是哥布林無結果風格
	q51->feedback.push_back("✅ 正確答案：A");
	q51->feedback.push_back("📘 解析：這類結局雖未修成正果，但保有角色成長與未來可能性，是成熟的 Bad Ending 設計方式，不讓玩家有過度挫敗感。");

	std::shared_ptr<Quiz> q52 = std::make_shared<Quiz>();
	q52->question = "題目 10. 情境：你希望設計一個 True Ending，讓玩家覺得「這場戀愛值得一切努力」。下列哪個演出最有效？";
	q52->options.push_back("A. 結尾兩人對話：「所以……我們現在，是戀人了嗎？」");
	q52->options.push_back("B. 畫面轉黑，只留下「感謝你玩到最後」字樣");
	q52->options.push_back("C. 兩人一起設計下一款戀愛遊戲，並暗示未來共事");
	q52->options.push_back("D. 角色給玩家一張紙條，上面寫著「再見」");
	q52->v_score = {5, 0, 10, 5}; // C是正確答案，B是哥布林無感結局
	q52->feedback.push_back("✅ 正確答案：C");
	q52->feedback.push_back("📘 解析：這類結局不只表示戀愛成功，也讓雙方在目標上同步，呈現感情成長與共同前景，是理想 True Ending 設計。");

	std::shared_ptr<Dialog> e1 = std::make_shared<Dialog>();
	e1->type = DialogType::GOODEND;
	e1->lines.push_back("✅ 成功攻略");
	e1->lines.push_back("【場景：美術系展演空間的角落，活動剛結束，你們坐在地板上，兩人靠得很近】");
	e1->lines.push_back("林夢瑤（輕聲）：「我一開始以為你只是想玩遊戲，沒想到你會陪我把整段劇情走完……還幫我補完那些我不敢寫的情節。」");
	e1->lines.push_back("你笑了笑，手裡還拿著那份你們一起完成的腳本。");
	e1->lines.push_back("主角：「因為那是我們兩個的故事，我不想讓它只停留在開頭。」");
	e1->lines.push_back("林夢瑤垂下眼睫，似乎有點不好意思。");
	e1->lines.push_back("林夢瑤：「那你覺得……我們的結局該怎麼寫？」");
	e1->lines.push_back("你望向她的眼睛，語氣認真：");
	e1->lines.push_back("主角：「這段劇情已經走到True Ending了，不用再選分支了。」");
	e1->lines.push_back("她愣了一下，然後笑了。");
	e1->lines.push_back("林夢瑤：「好，那我就把你寫進下一款遊戲裡……寫成一個讓我會心動的NPC。」");
	e1->lines.push_back("（畫面慢慢拉遠，燈光柔和，背景音樂響起）");
	e1->lines.push_back("「你成功通關了《遊戲程式與戀愛學特訓班》：林夢瑤路線｜攻略達成」");

	std::shared_ptr<Dialog> e2 = std::make_shared<Dialog>();
	e2->type = DialogType::BADEND;
	e2->lines.push_back("❌ 攻略失敗");
	e2->lines.push_back("【場景：空教室，桌面上只剩一張被退件的企劃書，主角靜靜坐著翻閱。黑板上的日期，是課程結束的前一天。】");
	e2->lines.push_back("你望著那份寫了一半的故事稿，裡面角色的對白停在一次爭吵之後，沒有結局。");
	e2->lines.push_back("老師的語音訊息在手機中播放——");
	e2->lines.push_back("劉焱成老師：「劇本寫到最後，如果沒有情感支撐，那就是一堆流程而已。」");
	e2->lines.push_back("你輕聲笑了一下，抬頭看著天花板。");
	e2->lines.push_back("主角 murmur：「果然還是太急了……想讓她喜歡上我，卻沒寫出她想要的劇情。」");
	e2->lines.push_back("這時，教室門口傳來熟悉的聲音。");
	e2->lines.push_back("林夢瑤（語氣平靜）：「我有看到你最近的設計，你有在進步。」");
	e2->lines.push_back("你轉頭，她正站在門邊，身後是微弱的走廊燈。");
	e2->lines.push_back("主角：「可是……我沒能幫你完成那個夢想的腳本。」");
	e2->lines.push_back("林夢瑤沉默了一下，然後遞出一支隨身碟。");
	e2->lines.push_back("林夢瑤：「那就留下來慢慢寫吧。不為通關，只是想和你……把故事寫完。」");
	e2->lines.push_back("（畫面慢慢轉暗，只剩窗邊微光）");
	e2->lines.push_back("「你未能通關《遊戲程式與戀愛學特訓班》：林夢瑤路線｜未攻略成功，但故事仍在繼續中……」");

	NPC& npc = DialogSystem::getInstance().addNPC(go, {d11, d12, q11, q12, d21, q21, q22, d31, q31, q32, d41, q41, q42, d51, q51, q52, e1, e2});
	npc.go->invMass = 0;
}

inline void initC(std::shared_ptr<GameObject> go)
{
	std::shared_ptr<Dialog> d11 = std::make_shared<Dialog>();
	d11->lines.push_back("主角（murmur）：「蛤……這傢伙的邏輯也太哲學系了吧？」");
	d11->lines.push_back("老師：「很好！這位是沈奕恆，他將帶領你進入心理學導向的戀愛互動設計之路。現在開始第一個教學模組。」");
	d11->lines.push_back("教學 1：角色視角的心理轉換是什麼？");
	d11->lines.push_back("【🎬 劇情】");
	d11->lines.push_back("教室燈光昏黃，沈奕恆正坐在最後一排，手裡翻著一本心理敘事學的書。你走進來，他抬頭看你一眼。");
	d11->lines.push_back("老師（推開門，手裡抱著幾本厚重教材）：「今天我們不講遊戲機制，我們講『視角』——不只是從哪個角度看故事，而是誰在感受這段故事。」");
	d11->lines.push_back("老師在白板上畫了兩個句子：");
	d11->lines.push_back("他看到她哭了，有點不知所措。");
	d11->lines.push_back("我看到她哭了，心臟像是被擰了一下。");
	d11->lines.push_back("老師轉頭問你：「你比較想玩哪一個角色？」");
	d11->lines.push_back("沈奕恆（淡淡開口）：「第一句像在看別人談戀愛，第二句……像是我在戀愛。」");
	d11->lines.push_back("老師：「這就是第一人稱的魔力。」");
	d11->lines.push_back("你愣了一下，試著低聲複誦：「我……心臟被擰了一下……」");
	d11->lines.push_back("沈奕恆（輕笑）：「不習慣了吧？不習慣進入角色心裡。但你得習慣，否則你做不出讓人心動的劇情。」");
	d11->lines.push_back("【📘 補充】");
	d11->lines.push_back("第三人稱（他/她）比較適合敘述劇情、觀察角色。");
	d11->lines.push_back("第一人稱（我）能讓玩家更直接帶入角色的情緒與思考。");
	d11->lines.push_back("沉浸式戀愛遊戲常使用第一人稱強化「我正在經歷這段戀情」的感覺。");

	std::shared_ptr<Quiz> q11 = std::make_shared<Quiz>();
	q11->question = "Q1. 你正在寫一段角色告白的台詞，哪一句最容易讓玩家產生共鳴？";
	q11->options.push_back("A. 他看著她，眼神中藏著情緒的風暴。");
	q11->options.push_back("B. 我看著她，眼神藏不住我胸口洶湧的情緒。");
	q11->options.push_back("C. 看著她，情緒有點複雜。");
	q11->options.push_back("D. 她低頭，他看著她沉默。");
	q11->v_score = {5, 10, 0, 5}; // B是正確答案，C是哥布林簡單描述
	q11->feedback.push_back("✅ 正解：B");
	q11->feedback.push_back("💬 沈奕恆（點頭）：「用『我』，讓玩家沒得逃。」");
	q11->ansIndex = 1;

	std::shared_ptr<Quiz> q12 = std::make_shared<Quiz>();
	q12->question = "Q2. 老師：「視角設計不是技術問題，是情感問題。」這句話的意思是？";
	q12->options.push_back("A. 遊戲應該多用鏡頭特效");
	q12->options.push_back("B. 玩家要能從角色立場感受愛情");
	q12->options.push_back("C. 劇情要全用旁白描述才合理");
	q12->options.push_back("D. 玩家應該只看劇情，不做選擇");
	q12->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林被動風格
	q12->feedback.push_back("✅ 正解：B");
	q12->feedback.push_back("💬 主角 murmur：「原來，不只是寫出來，而是要讓人心裡也動起來……」");
	q12->ansIndex = 1;

	std::shared_ptr<Dialog> d21 = std::make_shared<Dialog>();
	d21->lines.push_back("教學2：情緒迴圈與內隱選擇設計");
	d21->lines.push_back("主題：讓選擇影響情緒，而不是只影響劇情走向");
	d21->lines.push_back("學習目標：理解情緒如何在選擇中悄悄運作，塑造玩家與角色的心理連結");
	d21->lines.push_back("下課後，教室只剩你和沈奕恆。他靠著窗邊，手裡拿著飲料吸了一口，然後問了一句：");
	d21->lines.push_back("沈奕恆：「你喜歡那種選擇題，選 A 就戀愛成功、選 B 就失戀的遊戲嗎？」");
	d21->lines.push_back("你搖頭：「那太機械了，沒什麼感覺。」");
	d21->lines.push_back("他露出一抹幾不可見的微笑：「我也是。真正好的選項……不該告訴你結果，而是讓你去『感覺』角色當下會怎麼想。」");
	d21->lines.push_back("他走向講台，打開投影機。畫面顯示一個選擇分支圖，每個選項都標示著不同的角色情緒：「尷尬」「愧疚」「微妙喜歡」「不確定」。");
	d21->lines.push_back("沈奕恆：「這叫『情緒迴圈』，不是給你看到結局的選項，而是讓你在心裡自己走到那個情緒裡。」");
	d21->lines.push_back("你：「所以……我們不是選結局，而是選情緒？」");
	d21->lines.push_back("沈奕恆：「對。感情不是一瞬間發生的，是在一次次細小選擇中，被引導出來的。」");
	d21->lines.push_back("【📘 補充】");
	d21->lines.push_back("內隱選擇設計：選項表面看起來模糊，但其實暗藏情緒走向，引導玩家「體會」而非「知道」。");
	d21->lines.push_back("情緒迴圈：角色的情緒反應應該連續地影響下一個選擇，而不是重設。");
	d21->lines.push_back("範例：");
	d21->lines.push_back("Q：「收到她的訊息，你最自然的反應是？」");
	d21->lines.push_back("A：秒回（→情緒變得焦慮）");
	d21->lines.push_back("B：先假裝冷靜（→情緒變得壓抑）");
	d21->lines.push_back("C：等她問第二次（→情緒變得防衛）");
	d21->lines.push_back("這些選項不一定有對錯，但會形塑角色「走向哪種情感狀態」。");

	std::shared_ptr<Quiz> q21 = std::make_shared<Quiz>();
	q21->question = "Q1. 你要設計一個讓玩家感受到「被忽略」的戀愛選項，哪一個最有內隱情緒影響力？";
	q21->options.push_back("A. 不讀訊息");
	q21->options.push_back("B. 傳訊息說「晚點再說」");
	q21->options.push_back("C. 點開對方限動不回訊息");
	q21->options.push_back("D. 跟對方說「先忙」但其實沒事做");
	q21->v_score = {0, 5, 10, 5}; // C是正確答案，A是哥布林直接忽略
	q21->feedback.push_back("✅ 正解：C");
	q21->feedback.push_back("「這不是最直接的，但會讓人一直想『他是不是故意的』。這種模糊，才最傷人。」");
	q21->ansIndex = 2;

	std::shared_ptr<Quiz> q22 = std::make_shared<Quiz>();
	q22->question = "Q2.下列哪句敘事最能設計出讓玩家自己體會「遲疑中的心動」？";
	q22->options.push_back("A. 我告訴她我喜歡她了。");
	q22->options.push_back("B. 我本來想傳訊息，結果停在打字框好幾分鐘。");
	q22->options.push_back("C. 我立刻按下送出鍵。");
	q22->options.push_back("D. 她走過來，我轉身走開。");
	q22->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林逃避風格
	q22->feedback.push_back("✅ 正解：B");
	q22->feedback.push_back("💬 主角 murmur：「原來一個卡住的瞬間，也能讓人心臟砰砰跳。」");
	q22->ansIndex = 1;

	std::shared_ptr<Dialog> d31 = std::make_shared<Dialog>();
	d31->lines.push_back("教學 3：多重視角與心理張力設計");
	d31->lines.push_back("主題：讓玩家同時理解「角色在想什麼」與「玩家自己在感受什麼」");
	d31->lines.push_back("學習目標：學會如何設計「角色情緒視角」＋「玩家沉浸視角」的並行對位，強化情感緊繃與代入感");
	d31->lines.push_back("【🎬 劇情】");
	d31->lines.push_back("你和沈奕恆正在進行一項期末練習——用兩種視角寫一段「失約」的劇情：");
	d31->lines.push_back("一個是主角被放鴿子的視角");
	d31->lines.push_back("一個是放鴿子的那方視角");
	d31->lines.push_back("沈奕恆坐在你旁邊，低著頭打字，一言不發。你忍不住偷瞄他的螢幕，上面寫著：");
	d31->lines.push_back("「我明明也想去見他，但我真的不敢。我怕見了他，連保持距離這件事都做不到了。」");
	d31->lines.push_back("你心裡一震，剛想開口，他卻突然闔上筆電。");
	d31->lines.push_back("沈奕恆（語氣平靜）：「多重視角可以讓情感更厚實，但要小心使用。太快揭露，情緒會提早釋放完；太慢揭露，玩家會抽離。」");
	d31->lines.push_back("你問：「那你怎麼拿捏？」");
	d31->lines.push_back("沈奕恆（望著窗外）：");
	d31->lines.push_back("「靠張力。讓兩個視角的感覺互相矛盾、交錯，但又不完全對立。像一條看不到終點的拉鋸戰，才讓人上癮。」");
	d31->lines.push_back("【📘 補充】");
	d31->lines.push_back("多重視角敘事：同時給出「主角視角」與「他人視角」，但資訊不對等，營造心理緊繃");
	d31->lines.push_back("心理張力設計：不是用外在衝突（吵架、衝突）製造高潮，而是用「情感的未說出口」與「理解落差」創造壓抑與張力");
	d31->lines.push_back("範例：");
	d31->lines.push_back("玩家知道「沈奕恆其實很在意主角」，但主角卻誤會他冷漠");
	d31->lines.push_back("玩家面臨的選擇不是「衝出去表白」，而是「是否忍住、等待」");
	d31->lines.push_back("這類選擇能夠累積心理張力，為後續情感爆发打底。");

	std::shared_ptr<Quiz> q31 = std::make_shared<Quiz>();
	q31->question = "Q1. 你希望讓玩家在遊戲中同時感受到「他不來」與「他其實很在意」的矛盾效果，應該怎麼設計？";
	q31->options.push_back("A. 他傳訊息說「最近很忙」");
	q31->options.push_back("B. 他沒來，但桌上有一杯還溫熱的咖啡");
	q31->options.push_back("C. 他直接打來說「別等我」");
	q31->options.push_back("D. 他在訊息中打了一大串解釋");
	q31->v_score = {0, 10, 5, 5}; // B是正確答案，A是哥布林簡單藉口
	q31->feedback.push_back("✅ 正解：B");
	q31->feedback.push_back("💬 沈奕恆（低聲）：「溫度留下了他曾經在的證據……比千言萬語更難忘。」");
	q31->ansIndex = 1;

	std::shared_ptr<Quiz> q32 = std::make_shared<Quiz>();
	q32->question = "Q2. 你設計了一段兩人吵架的劇情，想讓玩家明白「沈奕恆其實在壓抑情緒」但表面冷靜，應該怎麼寫他的台詞？";
	q32->options.push_back("A. 「我沒事，你做什麼都可以。」");
	q32->options.push_back("B. 「我說了，這件事不重要。」");
	q32->options.push_back("C. 「……這樣也好，反正我們本來就不該太親近。」");
	q32->options.push_back("D. 「你想怎樣就怎樣。」");
	q32->v_score = {0, 5, 10, 5}; // C是正確答案，A是哥布林消極回應
	q32->feedback.push_back("✅ 正解：C");
	q32->feedback.push_back("💬 主角 murmur：「好像真的沒什麼，但哪裡……讓人心裡很悶。」");
	q32->ansIndex = 2;

	std::shared_ptr<Dialog> d41 = std::make_shared<Dialog>();
	d41->lines.push_back("教學 4：動態對話系統与角色記憶反應");
	d41->lines.push_back("主題：過去的選擇如何影響角色回應");
	d41->lines.push_back("學習目標：理解角色「記憶」與「回饋反應」在遊戲敘事中的設計方式，並學會為「情緒堆疊」留下伏筆");
	d41->lines.push_back("【🎬 劇情】");
	d41->lines.push_back("你這幾天跟沈奕恆的對話頻率越來越高。雖然他還是話少，但你總覺得，他好像記得你說過的每一句話。");
	d41->lines.push_back("今天在練習互動模擬，你故意輸入一句看似隨機的選項：");
	d41->lines.push_back("「那你會記得我說過的話嗎？」");
	d41->lines.push_back("沈奕恆愣了一下，然後淡淡地回：");
	d41->lines.push_back("「你不是說過你喜歡冷色調的封面設計嗎？我以為你也會比較喜歡這種回應方式。」");
	d41->lines.push_back("你一時說不出話來。原來，他真的都有記住。");
	d41->lines.push_back("老師經過，看見你們的設計稿，點點頭。");
	d41->lines.push_back("老師：「動態對話不是單純的『選項回應』，而是設計一種『有記憶的角色反應』——你今天對他怎麼說，他明天就會怎麼回答你。」");
	d41->lines.push_back("沈奕恆看著螢幕，輕聲補一句：");
	d41->lines.push_back("沈奕恆：「就像……你上次說過你害怕冷場，所以我才會現在主動說話。」");
	d41->lines.push_back("你忽然覺得胸口有點悶——明明只是程式設計課，為什麼感覺像是在談心？");
	d41->lines.push_back("【📘 補充】");
	d41->lines.push_back("動態對話系統：設計角色會記住玩家選項，並在後續互動中做出相應反應");
	d41->lines.push_back("記憶反应邏輯：玩家行為影響角色信任度、態度改變、甚至劇情走向");
	d41->lines.push_back("情緒堆疊的方式：讓角色慢慢記錄下玩家的選擇，讓後續爆發更有說服力（ex. 爆发前的沉默＝長期積累）");
	d41->lines.push_back("範例：");
	d41->lines.push_back("玩家曾選擇「忽略沈奕恆提到壓力」→ 之後沈奕恆在分組報告時選擇和別人合作");
	d41->lines.push_back("玩家曾選擇「主動詢問他壓力」→ 他在後期主動私訊：「這次報告……我想跟你一組。」");

	std::shared_ptr<Quiz> q41 = std::make_shared<Quiz>();
	q41->question = "Q1. 你希望讓沈奕恆根據玩家過去是否「主動關心」來決定是否講真話，哪種設計方式較好？";
	q41->options.push_back("A. 設定機率：關心過→30%會講真話");
	q41->options.push_back("B. 分兩種劇情線：關心過→進入沈奕恆回憶事件");
	q41->options.push_back("C. 讓玩家選項固定，劇情照常發展");
	q41->options.push_back("D. 加入沈奕恆說謊的選項，增加趣味性");
	q41->v_score = {5, 10, 0, 5}; // B是正確答案，C是哥布林無變化風格
	q41->feedback.push_back("✅ 正解：B");
	q41->feedback.push_back("💬 老師：「好設計不靠運氣，而是讓選擇變得值得。」");
	q41->ansIndex = 1;

	std::shared_ptr<Quiz> q42 = std::make_shared<Quiz>();
	q42->question = "Q2. 你設計了一段對話，想讓玩家從「沈奕恆的反應」中感受到他記得過去的互動，哪句台詞最適合？";
	q42->options.push_back("A. 「……沒什麼，就照流程走。」");
	q42->options.push_back("B. 「你那時不是說這樣會讓人沒安全感嗎？」");
	q42->options.push_back("C. 「嗯，我記不得了。」");
	q42->options.push_back("D. 「每次都這樣，也挺正常的。」");
	q42->v_score = {0, 10, 5, 5}; // B是正確答案，A是哥布林機械回應
	q42->feedback.push_back("✅ 正解：B");
	q42->feedback.push_back("💬 主角 murmur：「他說得很輕……但我記得我講過這句話是在……我們第一次吵架之後。」");

	std::shared_ptr<Dialog> d51 = std::make_shared<Dialog>();
	d51->lines.push_back("📍這段教學讓「情感記憶」逐漸浮現：沈奕恆雖然不主動，但一點一滴的累積，讓情緒的壓抑變得更真實、更有力。");
	d51->lines.push_back("教學 5：情緒崩潰點與玩家代入的情感爆發");
	d51->lines.push_back("主題：情感的崩潰與代入");
	d51->lines.push_back("學習目標：了解如何設計情感崩潰點，使玩家能夠深刻體會角色內心的掙扎與解放，並讓情感的爆發有說服力和衝擊力");
	d51->lines.push_back("【🎬 劇情】");
	d51->lines.push_back(
			"這幾天來，你和沈奕恆之間的對話越來越少，氣氛也逐漸變得有些緊張。你注意到，他的眼神變得更冷淡，甚至對你的問題也不再像以前那樣細心回答。");
	d51->lines.push_back("這一切似乎是無形中積累的結果，無論是小小的冷場，還是那個不經意的回應，漸漸地他似乎在逃避你。");
	d51->lines.push_back("今天你決定找沈奕恆，談一談這段時間的變化。你知道，這場對話可能會決定你們之間的未來。");
	d51->lines.push_back("沈奕恆（冷靜地）：「這幾天，我注意到你的變化。是不是在刻意疏遠我？」");
	d51->lines.push_back("你心頭一緊，低頭不語。這一刻，你終於體會到他心中的掙扎，原來他也在忍耐。");
	d51->lines.push_back("你回答：「其實，我在害怕。如果我一直靠近你，會不會讓你覺得負擔？你之前說過你討厭情感依賴，我擔心……會讓你更遠離我。」");
	d51->lines.push_back("沈奕恆愣住，眼中閃過一絲迷茫。隨後，他的表情變得更加複雜。");
	d51->lines.push_back("沈奕恆（低聲）：「你以為……我一直保持距離，是因為你依賴我嗎？不，我是因為……我自己不敢再靠近你。」");
	d51->lines.push_back("你感到一陣震驚，這句話像是敲響了你內心的鐘聲。你沒想到，沈奕恆一直在壓抑自己的情感，將內心的柔軟部分隱藏在冷靜的外表下。");
	d51->lines.push_back("沈奕恆（眼神變得堅定）：「這段時間，我一直在想我們之間的關係。每當我接近你，心裡總會有一種莫名的恐懼。那是……我不敢面對的情感。」");
	d51->lines.push_back("你感受到一種強烈的情感波動，這不僅僅是角色之間的對話，更像是心靈深處的碰撞。");
	d51->lines.push_back("沈奕恆：「我一直以為，控制情感是最安全的方式，但現在我知道，我不能再繼續這樣逃避下去。」");
	d51->lines.push_back("【📘 補充】");
	d51->lines.push_back("情感爆發點的設計：情感崩潰點是情感積壓後的釋放，需注意時機與玩家代入感，讓情緒的衝擊有足夠的張力。");
	d51->lines.push_back("角色心境反差：沈奕恆在整個劇情過程中，表現為冷靜理性，直到這一刻，突然表露出他內心的脆弱與無力感，這是情感爆發的關鍵。");
	d51->lines.push_back("代入感：玩家將沈奕恆的冷淡與矛盾視為一種內心的自我防衛，隨著情感爆發，玩家與角色的情感連結會更加緊密。");

	std::shared_ptr<Quiz> q51 = std::make_shared<Quiz>();
	q51->question = "Q1. 當設計情感崩潰點時，以下哪個元素最能增強情感的爆發力？";
	q51->options.push_back("A. 強烈的衝突與對抗");
	q51->options.push_back("B. 輕描淡寫的反應，讓情感逐漸浮現");
	q51->options.push_back("C. 一個突然的、戲劇性的事件");
	q51->options.push_back("D. 玩家無法選擇的情節，讓角色的情感主導一切");
	q51->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林被動接受
	q51->feedback.push_back("✅ 正解：B");
	q51->feedback.push_back("💬 老師：「情感的爆發應該是逐步累積的，這樣才有足夠的衝擊力。」");
	q51->ansIndex = 1;

	std::shared_ptr<Quiz> q52 = std::make_shared<Quiz>();
	q52->question = "Q2. 若想讓沈奕恆的情感崩潰更具震撼感，哪種設計最能提升效果？";
	q52->options.push_back("A. 讓玩家選擇是否解開沈奕恆的內心");
	q52->options.push_back("B. 讓沈奕恆主動揭示自己的情感過程，帶有回憶的情感描寫");
	q52->options.push_back("C. 讓沈奕恆保持冷漠，直到最終揭示內心");
	q52->options.push_back("D. 讓沈奕恆在玩家的選擇中始終保持淡定");
	q52->v_score = {5, 10, 5, 0}; // B是正確答案，D是哥布林無反應風格
	q52->feedback.push_back("✅ 正解：B");
	q52->feedback.push_back("💬 主角 murmur：「這段時間我一直以為他冷漠，沒想到……是他在掙扎、在逃避。」");
	q52->ansIndex = 1;

	std::shared_ptr<Dialog> e1 = std::make_shared<Dialog>();
	e1->type = DialogType::GOODEND;
	e1->lines.push_back("成功攻略沈奕恆");
	e1->lines.push_back("你們終於突破了那層無形的障礙，沈奕恆的情感終於被釋放，他不再壓抑自己的情感，兩人之間的關係終於有了突破。");
	e1->lines.push_back("你與沈奕恆站在教室窗前，看著外面漸漸暗下來的天空。你們的眼神交會，彼此之間不再有疏離感，只有無言的默契。");
	e1->lines.push_back("沈奕恆（輕聲）：「或許，我們不需要再理智到冷血。只要你能在我身邊，我就夠了。」");
	e1->lines.push_back("你們的手指微微碰觸，彼此都感受到對方內心的那份溫暖。");
	e1->lines.push_back("（畫面漸暗，顯示文字）");
	e1->lines.push_back("「你成功通關了《遊戲程式與戀愛學特訓班》：沈奕恆路線｜攻略達成」");

	std::shared_ptr<Dialog> e2 = std::make_shared<Dialog>();
	e2->type = DialogType::BADEND;
	e2->lines.push_back("攻略失敗");
	e2->lines.push_back("沈奕恆站在你面前，沉默片刻。你能感受到他內心的掙扎，卻依然無法觸及到他的內心。");
	e2->lines.push_back("沈奕恆（低頭）：「或許，我不應該再繼續這樣逃避。我不擅長表達情感，但這不意味著我不在乎。」");
	e2->lines.push_back("你聽見他輕聲自語，心中有一種說不清的遺憾。也許，他的心門永遠無法打開，或許，你還需要更長時間來解開他心中的結。");
	e2->lines.push_back("（畫面轉黑，顯示文字）");
	e2->lines.push_back("「你未能通關《遊戲程式與戀愛學特訓班》：沈奕恆路線｜未攻略成功，但故事還沒結束……？」");

	NPC& npc = DialogSystem::getInstance().addNPC(go, {d11, q11, q12, d21, q21, q22, d31, q31, q32, d41, q41, q42, d51, q51, q52, e1, e2});
	npc.go->invMass = 0;
}