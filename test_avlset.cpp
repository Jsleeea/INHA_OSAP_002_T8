#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

#include "src/AVLSet.cpp"

namespace {
std::string CaptureStdout(const std::function<void()> &fn) {
  std::ostringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  try {
    fn();
  } catch (...) {
    std::cout.rdbuf(old);
    throw;
  }
  std::cout.rdbuf(old);
  return buffer.str();
}

std::string OneToken(const std::string &out) {
  istringstream iss(out);
  string tok;
  iss >> tok;
  return tok;
}

} // namespace

// ----------------------------------------------------------
// -----------------------<테스트 시작>-------------------------
// ----------------------------------------------------------

// 텍스트 픽스쳐
class AVLSetTest : public ::testing::Test {
protected:
  using Node = AvlSet::Node;

  // 공통 AvlSet 객체
  AvlSet s;
};

// -------------------------Node 구조체 테스트--------------------------

// Node 구조체 초기값 테스트
TEST_F(AVLSetTest, NodeInitialization) {
  int test_key = 10;

  // Node 생성 (부모 노드 없이)
  Node *node = new Node(test_key);

  // Key 값 확인
  EXPECT_EQ(node->key, test_key);

  // 초기 Height = 1
  EXPECT_EQ(node->height, 1);

  // 초기 Size = 1
  EXPECT_EQ(node->size, 1);

  // 자식, 부모 포인터 = nullptr
  EXPECT_EQ(node->left, nullptr);
  EXPECT_EQ(node->right, nullptr);
  EXPECT_EQ(node->parent, nullptr);

  delete node;
}

// Node 구조체 부모 포인터 존재시 테스트
TEST_F(AVLSetTest, NodeInitializationWithParent) {
  Node *parent_node = new Node(20);
  int child_key = 10;

  // 부모를 지정 Node 생성
  Node *child_node = new Node(child_key, parent_node);

  // 부모 포인터 연결 확인
  EXPECT_EQ(child_node->parent, parent_node);
  EXPECT_EQ(child_node->key, child_key);

  delete child_node;
  delete parent_node;
}

// -------------------------BalanceDegree 테스트--------------------------
// 단순 케이스 테스트
TEST_F(AVLSetTest, BalanceDegree_Basic) {
  // nullptr 인 경우 0 반환
  EXPECT_EQ(s.BalanceDegree(nullptr), 0);

  // 자식이 없는 노드인 경우 0 반환
  Node *node = new Node(10);
  EXPECT_EQ(s.BalanceDegree(node), 0);
  delete node;
}

class BalanceDegreeParamTest
    : public AVLSetTest,
      public ::testing::WithParamInterface<std::tuple<int, int, int>> {};

TEST_P(BalanceDegreeParamTest, CheckBalanceCalculation) {

  int left_h = std::get<0>(GetParam());
  int right_h = std::get<1>(GetParam());
  int expected_balance = std::get<2>(GetParam());

  Node *root = new Node(100);

  // 왼쪽 자식 설정
  if (left_h > 0) {
    root->left = new Node(50);
    root->left->parent = root;
    root->left->height = left_h; // 높이 설정
  }

  // 오른쪽 자식 설정
  if (right_h > 0) {
    root->right = new Node(150);
    root->right->parent = root;
    root->right->height = right_h; // 높이 설정
  }

  // BalanceDegree (왼쪽 높이 - 오른쪽 높이)
  EXPECT_EQ(s.BalanceDegree(root), expected_balance);

  if (root->left)
    delete root->left;
  if (root->right)
    delete root->right;
  delete root;
}

INSTANTIATE_TEST_SUITE_P(
    BalanceDegreeVariations, BalanceDegreeParamTest,
    ::testing::Values(
        // 값: (Left Height, Right Height, Expected Result)
        std::make_tuple(0, 0, 0),  // 자식 없음
        std::make_tuple(1, 0, 1),  // 왼쪽만 존재 -> Balance 1
        std::make_tuple(0, 1, -1), // 오른쪽만 존재 -> Balance -1
        std::make_tuple(1, 1, 0),  // 양쪽 동일 -> Balance 0
        std::make_tuple(2, 1, 1),  // 왼쪽이 1 더 큼
        std::make_tuple(1, 2, -1), // 오른쪽이 1 더 큼
        std::make_tuple(3, 1, 2),  // 왼쪽으로 크게 치우침
        std::make_tuple(1, 3, -2)  // 오른쪽으로 크게 치우침
        ));

// -------------------------ResizeHs 테스트--------------------------

// 단순 케이스 테스트
TEST_F(AVLSetTest, ResizeHs_Basic) {
  s.ResizeHs(nullptr);

  Node *node = new Node(10);

  // 테스트를 위해 일부러 잘못된 값 설정
  node->height = 999;
  node->size = 999;

  // ResizeHs 호출 (자식이 없으므로 height=1, size=1로 복구됨 확인)
  s.ResizeHs(node);

  EXPECT_EQ(node->height, 1);
  EXPECT_EQ(node->size, 1);

  delete node;
}


//튜플 구조: <int lh, int ls, int rh, int rs, int exp_h, int exp_s>
class ResizeHsParamTest : public AVLSetTest,
                          public ::testing::WithParamInterface<
                              std::tuple<int, int, int, int, int, int>> {};

TEST_P(ResizeHsParamTest, CheckResizeUpdate) {

  int lh = std::get<0>(GetParam());
  int ls = std::get<1>(GetParam());
  int rh = std::get<2>(GetParam());
  int rs = std::get<3>(GetParam());
  int exp_h = std::get<4>(GetParam());
  int exp_s = std::get<5>(GetParam());

  Node *root = new Node(100);

  // 왼쪽 자식 설정
  if (lh > 0 || ls > 0) {
    root->left = new Node(50);
    root->left->parent = root;
    root->left->height = lh;
    root->left->size = ls;
  }

  // 오른쪽 자식 설정
  if (rh > 0 || rs > 0) {
    root->right = new Node(150);
    root->right->parent = root;
    root->right->height = rh;
    root->right->size = rs;
  }

  s.ResizeHs(root);

  EXPECT_EQ(root->height, exp_h);
  EXPECT_EQ(root->size, exp_s);

  if (root->left)
    delete root->left;
  if (root->right)
    delete root->right;
  delete root;
}

INSTANTIATE_TEST_SUITE_P(
    ResizeHsVariations, ResizeHsParamTest,
    ::testing::Values(
        // (LH, LS, RH, RS, expected_height, expected_size)

        // 자식이 없는 경우
        // Height = 1, Size = 1
        std::make_tuple(0, 0, 0, 0, 1, 1),

        // 왼쪽 자식만 있는 경우 (LH=2, LS=3)
        // Height = 1 + 2 = 3, Size = 1 + 3 = 4
        std::make_tuple(2, 3, 0, 0, 3, 4),

        // 오른쪽 자식만 있는 경우 (RH=3, RS=5)
        // Height = 1 + 3 = 4, Size = 1 + 5 = 6
        std::make_tuple(0, 0, 3, 5, 4, 6),

        // 양쪽 자식 존재
        // L(H=3, S=4), R(H=1, S=1)
        // Height = 1 + max(3, 1) = 4, Size = 1 + 4 + 1 = 6
        std::make_tuple(3, 4, 1, 1, 4, 6),

        // 양쪽 자식 존재
        // L(H=2, S=2), R(H=4, S=8)
        // Height = 1 + max(2, 4) = 5, Size = 1 + 2 + 8 = 11
        std::make_tuple(2, 2, 4, 8, 5, 11),

        // 양쪽 높이 동일
        // L(H=2, S=3), R(H=2, S=3)
        // Height = 1 + 2 = 3, Size = 1 + 3 + 3 = 7
        std::make_tuple(2, 3, 2, 3, 3, 7)));

// -------------------------ReBalance 테스트--------------------------

// 4가지 회전 케이스(LL, RR, LR, RL)
enum RotationCase { LL, RR, LR, RL };

class ReBalanceParamTest : public AVLSetTest,
                           public ::testing::WithParamInterface<RotationCase> {
};

TEST_P(ReBalanceParamTest, CheckRotations) {
  RotationCase type = GetParam();

  // 테스트 노드들 (Key: 10, 20, 30)
  Node *n10 = new Node(10);
  Node *n20 = new Node(20);
  Node *n30 = new Node(30);

  Node *start_node = nullptr;

  if (type == LL) {
    // LL Case: 30 -> 20 -> 10
    s.root_ = n30;
    n30->left = n20;
    n20->parent = n30;
    n20->left = n10;
    n10->parent = n20;

    // ReBalance 시작점
    start_node = n10;
  } else if (type == RR) {
    // RR Case: 10 -> 20 -> 30
    s.root_ = n10;
    n10->right = n20;
    n20->parent = n10;
    n20->right = n30;
    n30->parent = n20;

    start_node = n30;
  } else if (type == LR) {
    // LR Case: 30 -> 10 -> 20
    s.root_ = n30;
    n30->left = n10;
    n10->parent = n30;
    n10->right = n20;
    n20->parent = n10;

    start_node = n20;
  } else if (type == RL) {
    // RL Case: 10 -> 30 -> 20
    s.root_ = n10;
    n10->right = n30;
    n30->parent = n10;
    n30->left = n20;
    n20->parent = n30;

    start_node = n20;
  }

  // 균형 맞추기
  // start_node부터 부모로 올라가며 ResizeHs, 회전 수행
  s.ReBalance(start_node);

  // 회전 결과가 20을 루트로 하고 10, 30을 자식으로 가지는지

  // 루트 노드 확인 (20)
  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, 20);
  EXPECT_EQ(s.root_->parent, nullptr);
  EXPECT_EQ(s.root_->height, 2); // 높이: 2
  EXPECT_EQ(s.root_->size, 3);   // 사이즈: 3

  // 왼쪽 자식 확인 (10)
  ASSERT_NE(s.root_->left, nullptr);
  EXPECT_EQ(s.root_->left->key, 10);
  EXPECT_EQ(s.root_->left->parent, s.root_);
  EXPECT_EQ(s.root_->left->height, 1);
  EXPECT_EQ(s.root_->left->size, 1);
  EXPECT_EQ(s.root_->left->left, nullptr);
  EXPECT_EQ(s.root_->left->right, nullptr);

  // 오른쪽 자식 확인 (30)
  ASSERT_NE(s.root_->right, nullptr);
  EXPECT_EQ(s.root_->right->key, 30);
  EXPECT_EQ(s.root_->right->parent, s.root_);
  EXPECT_EQ(s.root_->right->height, 1);
  EXPECT_EQ(s.root_->right->size, 1);
  EXPECT_EQ(s.root_->right->left, nullptr);
  EXPECT_EQ(s.root_->right->right, nullptr);

  delete n10;
  delete n20;
  delete n30;
  s.root_ = nullptr;
}

INSTANTIATE_TEST_SUITE_P(ReBalanceVariations, ReBalanceParamTest,
                         ::testing::Values(LL, RR, LR, RL));

// -------------------------Rotate 함수 테스트--------------------------

// RotateLeft 테스트
// 상황: 10 -> 20 (10이 루트, 20이 오른쪽 자식)
// 기대: 20 -> 10 (20이 루트, 10이 왼쪽 자식)
TEST_F(AVLSetTest, RotateLeft_Root) {
  Node *n10 = new Node(10);
  Node *n20 = new Node(20);

  s.root_ = n10;
  n10->right = n20;
  n20->parent = n10;

  // 회전
  Node *new_root = s.RotateLeft(n10);

  //반환된 노드가 새로운 루트여야 함
  EXPECT_EQ(new_root, n20);
  EXPECT_EQ(s.root_, n20);

  //부모,자식 관계 역전 확인
  EXPECT_EQ(n20->parent, nullptr);
  EXPECT_EQ(n20->left, n10);
  EXPECT_EQ(n10->parent, n20);
  EXPECT_EQ(n10->right, nullptr);

  //높이와 사이즈 갱신 확인
  EXPECT_EQ(n10->height, 1);
  EXPECT_EQ(n10->size, 1);

  EXPECT_EQ(n20->height, 2);
  EXPECT_EQ(n20->size, 2);

  delete n10;
  delete n20;
  s.root_ = nullptr;
}

// 2. RotateRight 테스트
// 상황: 20 -> 10 (20이 루트, 10이 왼쪽 자식)
// 기대: 10 -> 20 (10이 루트, 20이 오른쪽 자식)
TEST_F(AVLSetTest, RotateRight_Root) {
  Node *n10 = new Node(10);
  Node *n20 = new Node(20);

  s.root_ = n20;
  n20->left = n10;
  n10->parent = n20;

  // 회전
  Node *new_root = s.RotateRight(n20);

  EXPECT_EQ(new_root, n10);
  EXPECT_EQ(s.root_, n10);

  EXPECT_EQ(n10->parent, nullptr);
  EXPECT_EQ(n10->right, n20);
  EXPECT_EQ(n20->parent, n10);
  EXPECT_EQ(n20->left, nullptr);

  // 높이,사이즈 확인
  EXPECT_EQ(n20->height, 1);
  EXPECT_EQ(n10->height, 2);

  delete n10;
  delete n20;
  s.root_ = nullptr;
}

// RotateLeft 복합 테스트 (서브트리 이동 및 부모 연결 확인)
TEST_F(AVLSetTest, RotateLeft_General_WithSubtree) {
  Node *p = new Node(5);
  Node *x = new Node(10);
  Node *y = new Node(20);
  Node *b = new Node(15);

  s.root_ = p;

  p->right = x;
  x->parent = p;
  x->right = y;
  y->parent = x;
  y->left = b;
  b->parent = y;

  // 회전 실행 (X를 기준 좌회전)
  Node *new_sub_root = s.RotateLeft(x);

  // 리턴값은 Y여야 함
  EXPECT_EQ(new_sub_root, y);

  // P(부모)와의 연결 확인
  EXPECT_EQ(p->right, y);
  EXPECT_EQ(y->parent, p);

  // 서브트리 B(15)의 이동 확인
  EXPECT_EQ(x->right, b);
  EXPECT_EQ(b->parent, x);

  // X와 Y의 관계 확인
  EXPECT_EQ(y->left, x);
  EXPECT_EQ(x->parent, y);

  EXPECT_EQ(b->size, 1);
  EXPECT_EQ(x->size, 2);
  EXPECT_EQ(y->size, 3);

  delete p;
  delete x;
  delete y;
  delete b;
  s.root_ = nullptr;
}

// RotateRight 복합 테스트 (서브트리 이동, 부모 연결 확인)
// 동작: Y를 기준으로 RotateRight
// 기대: P의 왼쪽 자식이 X가 됨, B는 Y의 왼쪽 자식이 됨.
TEST_F(AVLSetTest, RotateRight_General_WithSubtree) {
  Node *p = new Node(30);
  Node *y = new Node(20);
  Node *x = new Node(10);
  Node *b = new Node(15);

  s.root_ = p;
  p->left = y;
  y->parent = p;
  y->left = x;
  x->parent = y;
  x->right = b;
  b->parent = x;

  // 회전 (Y를 기준으로 우회전)
  Node *new_sub_root = s.RotateRight(y);

  // 리턴값: X
  EXPECT_EQ(new_sub_root, x);

  // P(부모)와의 연결
  EXPECT_EQ(p->left, x);
  EXPECT_EQ(x->parent, p);

  // 서브트리 B(15)의 이동 (X의 오른쪽 -> Y의 왼쪽)
  EXPECT_EQ(y->left, b);
  EXPECT_EQ(b->parent, y);

  // X와 Y의 관계
  EXPECT_EQ(x->right, y);
  EXPECT_EQ(y->parent, x);

  // 높이 업데이트 확인
  EXPECT_EQ(b->height, 1);
  EXPECT_EQ(y->height, 2);
  EXPECT_EQ(x->height, 3);

  delete p;
  delete y;
  delete x;
  delete b;
  s.root_ = nullptr;
}

// -------------------------FindNode 테스트--------------------------

TEST_F(AVLSetTest, FindNode_EmptyTree) {
  EXPECT_EQ(s.FindNode(10), nullptr);
}

TEST_F(AVLSetTest, FindNode_ComplexStructure) {

  Node *n20 = new Node(20);
  Node *n10 = new Node(10);
  Node *n30 = new Node(30);
  Node *n25 = new Node(25);
  Node *n40 = new Node(40);

  s.root_ = n20;

  // 20 -> 10, 30
  n20->left = n10;
  n10->parent = n20;
  n20->right = n30;
  n30->parent = n20;

  // 30 -> 25, 40
  n30->left = n25;
  n25->parent = n30;
  n30->right = n40;
  n40->parent = n30;

  // Root 노드 검색
  EXPECT_EQ(s.FindNode(20), n20);

  // 왼쪽 자식 노드 검색
  EXPECT_EQ(s.FindNode(10), n10);

  // 오른쪽 자식 노드 검색
  EXPECT_EQ(s.FindNode(30), n30);

  // 깊이가 있는 노드 검색 (왼쪽 이동 -> 오른쪽 이동 등 확인)
  EXPECT_EQ(s.FindNode(25), n25);
  EXPECT_EQ(s.FindNode(40), n40);

  // 존재하지 않는 노드 검색
  EXPECT_EQ(s.FindNode(5), nullptr);
  EXPECT_EQ(s.FindNode(15), nullptr);
  EXPECT_EQ(s.FindNode(50), nullptr);
  EXPECT_EQ(s.FindNode(35), nullptr);

  delete n20;
  delete n10;
  delete n30;
  delete n25;
  delete n40;
  s.root_ = nullptr;
}

// -------------------------Find 테스트--------------------------

TEST_F(AVLSetTest, Find_Functionality) {
  // 초기 상태: Empty
  const string out_empty = CaptureStdout([&] { s.Find(10); });
  EXPECT_EQ("-1", OneToken(out_empty)); // Not Found: -1 출력

  // Insert를 통해 데이터 삽입
  CaptureStdout([&] {
    s.Insert(30);
    s.Insert(20);
    s.Insert(10);
  });

  // 현재 트리 구조: 20(root), 10(left), 30(right)

  // 존재하는 노드 검색
  const string out_found_root = CaptureStdout([&] { s.Find(20); });
  EXPECT_EQ("0", OneToken(out_found_root));

  const string out_found_left = CaptureStdout([&] { s.Find(10); });
  EXPECT_EQ("1", OneToken(out_found_left));

  const string out_found_right = CaptureStdout([&] { s.Find(30); });
  EXPECT_EQ("1", OneToken(out_found_right));

  // 존재하지 않는 노드 검색
  const string out_not_found_min = CaptureStdout([&] { s.Find(5); });
  EXPECT_EQ("-1", OneToken(out_not_found_min)); // Not Found: -1 출력

  const string out_not_found_mid = CaptureStdout([&] { s.Find(15); });
  EXPECT_EQ("-1", OneToken(out_not_found_mid));

  const string out_not_found_max = CaptureStdout([&] { s.Find(35); });
  EXPECT_EQ("-1", OneToken(out_not_found_max));

}

// -------------------------Insert 테스트--------------------------

// 단순 삽입, 출력 테스트
TEST_F(AVLSetTest, Insert_Simple) {
  // 첫 번째 노드 삽입 (Root)
  string out1 = CaptureStdout([&] { s.Insert(10); });
  EXPECT_EQ("0", OneToken(out1));

  EXPECT_EQ(s.n_, 1);
  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, 10);
  EXPECT_EQ(s.root_->height, 1);

  // 두 번째 노드 삽입 (Left Child)
  string out2 = CaptureStdout([&] { s.Insert(5); });
  EXPECT_EQ("1", OneToken(out2));

  EXPECT_EQ(s.n_, 2);
  EXPECT_EQ(s.root_->height, 2);
  EXPECT_EQ(s.root_->left->key, 5);

  // 세 번째 노드 삽입 (Right Child)
  string out3 = CaptureStdout([&] { s.Insert(15); });
  EXPECT_EQ("1", OneToken(out3));

  EXPECT_EQ(s.n_, 3);
  EXPECT_EQ(s.root_->right->key, 15);
}

// -------------------------Insert Rotation & Output 테스트----------------
using InsertRotationParams = std::tuple<std::vector<int>, std::string, int>;

class InsertRotationParamTest
    : public AVLSetTest,
      public ::testing::WithParamInterface<InsertRotationParams> {};

TEST_P(InsertRotationParamTest, VerifyRotationAndOutput) {
  auto keys = std::get<0>(GetParam());
  string expected_last_output = std::get<1>(GetParam());
  int expected_root = std::get<2>(GetParam());

  for (size_t i = 0; i < keys.size() - 1; ++i) {
    CaptureStdout([&] { s.Insert(keys[i]); });
  }

  // 마지막 키 삽입
  string out = CaptureStdout([&] { s.Insert(keys.back()); });

  // 출력값 검증
  EXPECT_EQ(expected_last_output, OneToken(out))
      << "Output mismatch for insertion of " << keys.back();

  // 트리 구조 검증 (공통적으로 10, 20, 30 노드를 가짐)
  // 최종 상태: 완전한 균형 트리 (Root: 20, Left: 10, Right: 30)

  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, expected_root);
  EXPECT_EQ(s.root_->height, 2);
  EXPECT_EQ(s.root_->size, 3);

  // 왼쪽 자식 (10)
  ASSERT_NE(s.root_->left, nullptr);
  EXPECT_EQ(s.root_->left->key, 10);
  EXPECT_EQ(s.root_->left->parent, s.root_);
  EXPECT_EQ(s.root_->left->height, 1); // Leaf

  // 오른쪽 자식 (30)
  ASSERT_NE(s.root_->right, nullptr);
  EXPECT_EQ(s.root_->right->key, 30);
  EXPECT_EQ(s.root_->right->parent, s.root_);
  EXPECT_EQ(s.root_->right->height, 1); // Leaf
}

INSTANTIATE_TEST_SUITE_P(
    RotationScenarios, InsertRotationParamTest,
    ::testing::Values(
        // 1. LL Case (Right Rotation)
        std::make_tuple(std::vector<int>{30, 20, 10}, "1", 20),

        // 2. RR Case (Left Rotation)
        std::make_tuple(std::vector<int>{10, 20, 30}, "1", 20),

        // 3. LR Case (Left-Right Rotation)
        std::make_tuple(std::vector<int>{30, 10, 20}, "0", 20),

        // 4. RL Case (Right-Left Rotation)
        std::make_tuple(std::vector<int>{10, 30, 20}, "0", 20)));

// -------------------------Empty 테스트--------------------------

TEST_F(AVLSetTest, Empty_StateTransitions) {
  // 초기 상태 확인
  EXPECT_EQ("1", OneToken(CaptureStdout([&] { s.Empty(); })));

  // 데이터 삽입 후 확인
  CaptureStdout([&] { s.Insert(10); });
  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Empty(); })));

  // 데이터 추가 삽입 후 확인
  CaptureStdout([&] { s.Insert(20); });
  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Empty(); })));

  // 모든 데이터 삭제 후 확인 (원상 복구)
  CaptureStdout([&] { s.Erase(10); });
  CaptureStdout([&] { s.Erase(20); });

  EXPECT_EQ("1", OneToken(CaptureStdout([&] { s.Empty(); })));
}

// -------------------------Size 테스트--------------------------

TEST_F(AVLSetTest, Size_Tracking) {
  // 초기 사이즈 확인
  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Size(); })));

  // 10 삽입 -> Size 1
  CaptureStdout([&] { s.Insert(10); });
  EXPECT_EQ("1", OneToken(CaptureStdout([&] { s.Size(); })));

  // 20 삽입 -> Size 2
  CaptureStdout([&] { s.Insert(20); });
  EXPECT_EQ("2", OneToken(CaptureStdout([&] { s.Size(); })));

  // 30 삽입 -> Size 3
  CaptureStdout([&] { s.Insert(30); });
  EXPECT_EQ("3", OneToken(CaptureStdout([&] { s.Size(); })));

  // 20 삭제 -> Size 2
  CaptureStdout([&] { s.Erase(20); });
  EXPECT_EQ("2", OneToken(CaptureStdout([&] { s.Size(); })));

  // 10 삭제 -> Size 1
  CaptureStdout([&] { s.Erase(10); });
  EXPECT_EQ("1", OneToken(CaptureStdout([&] { s.Size(); })));

  // 30 삭제 -> Size 0
  CaptureStdout([&] { s.Erase(30); });
  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Size(); })));
}

// -------------------------Prev & Next 테스트--------------------------

class PrevNextTest : public AVLSetTest {
protected:
  void SetUp() override {

    CaptureStdout([&] {
      s.Insert(20);
      s.Insert(10);
      s.Insert(30);
      s.Insert(5);
      s.Insert(15);
      s.Insert(25);
      s.Insert(40);
    });
  }

  std::string GetTrimmedOutput(std::function<void()> func) {
    std::string out = CaptureStdout(func);
    if (!out.empty() && out.back() == '\n') {
      out.pop_back();
    }
    return out;
  }
};

TEST_F(PrevNextTest, Prev_scenarios) {
  // 왼쪽 자식이 있는 경우
  // 20의 왼쪽 서브트리중 가장 큰 값 -> 15
  // 15: Depth=2, Height=1 -> 출력 "15 2"
  EXPECT_EQ("15 2", GetTrimmedOutput([&] { s.Prev(20); }));

  // 왼쪽 자식이 없는 경우
  // 부모(10)의 오른쪽 자식이므로, 부모인 10이 Prev.
  // 10: Depth=1, Height=2 -> 출력 "10 2"
  EXPECT_EQ("10 2", GetTrimmedOutput([&] { s.Prev(15); }));

  // 왼쪽 자식이 없고, 부모를 타고 올라가야 하는 경우
  // 25는 Leaf 부모(30)의 왼쪽 자식 -> 더 위
  // 30은 부모(20)의 오른쪽 자식 -> 20이 Prev
  // 20: Depth=0, Height=3 -> 출력 "20 0"
  EXPECT_EQ("20 0", GetTrimmedOutput([&] { s.Prev(25); }));

  // Prev가 존재하지 않는 경우
  // Prev 없음 -> 출력 "-1"
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.Prev(5); }));
}

TEST_F(PrevNextTest, Next_scenarios) {
  // 오른쪽 자식이 있는 경우
  // 20의 오른쪽 서브트리(30) 중 가장 작은 값 -> 25
  // 25: Depth=2, Height=1 -> 출력 "25 2"
  EXPECT_EQ("25 2", GetTrimmedOutput([&] { s.Next(20); }));

  // 오른쪽 자식이 없는 경우
  // 25는 Leaf
  // 부모(30)의 왼쪽 자식이므로, 부모인 30이 Next
  // 30: Depth=1, Height=2 -> 출력 "30 2"
  EXPECT_EQ("30 2", GetTrimmedOutput([&] { s.Next(25); }));

  // 오른쪽 자식이 없고, 부모를 타고 올라가야 하는 경우
  // 15는 Leaf 부모(10)의 오른쪽 자식 -> 더 위
  // 10은 부모(20)의 왼쪽 자식 -> 20이 Next가 됨.
  // 20: Depth=0, Height=3 -> 출력 "20 0"
  EXPECT_EQ("20 0", GetTrimmedOutput([&] { s.Next(15); }));

  // Next가 존재하지 않는 경우
  // 40은 Leaf 부모(30)의 오른쪽 -> 부모(20)의 오른쪽 -> Root 도달
  // Next 없음 -> 출력 "-1"
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.Next(40); }));
}

// -------------------------UpperBound 테스트--------------------------

// 구조: {5, 10, 15, 20(Root), 25, 30, 40}

TEST_F(PrevNextTest, UpperBound_scenarios) {
  // 일반적인 경우
  // x=12 12보다 큰 가장 작은 값은 15
  // 15: Depth=2, Height=1 -> 출력 "15 2"
  EXPECT_EQ("15 2", GetTrimmedOutput([&] { s.UpperBound(12); }));

  // x가 트리에 존재하는 경우
  // x=20. 20보다 큰 가장 작은 값은 25
  // 25: Depth=2, Height=1 -> 출력 "25 2"
  EXPECT_EQ("25 2", GetTrimmedOutput([&] { s.UpperBound(20); }));

  // x가 트리 내의 최대값인 경우
  // x=40. 40보다 큰 값은 없음
  // 출력 "-1"
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.UpperBound(40); }));

  // x가 최대값을 초과하는 경우
  // x=50. 50보다 큰 값은 없음
  // 출력 "-1"
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.UpperBound(50); }));

  // x가 트리 내의 최소값보다 작은 경우
  // x=2. 2보다 큰 가장 작은 값은 5
  // 5: Depth=2, Height=1 -> 출력 "5 2"
  EXPECT_EQ("5 2", GetTrimmedOutput([&] { s.UpperBound(2); }));

  // x가 Root보다 약간 작은 경우
  // x=17. 17보다 큰 가장 작은 값은 20 (Root)
  // 20: Depth=0, Height=3 -> 출력 "20 0"
  EXPECT_EQ("20 0", GetTrimmedOutput([&] { s.UpperBound(17); }));
}

TEST_F(AVLSetTest, UpperBound_EmptyTree) {
  // 빈 트리에서 검색
  EXPECT_EQ("-1", OneToken(CaptureStdout([&] { s.UpperBound(10); })));
}

// -------------------------Rank 테스트--------------------------

TEST_F(PrevNextTest, Rank_Found) {
  // 최소값 (Key: 5) -> Rank 1
  // Leaf (Depth 2, Height 1) -> 2 * 1 = 2
  // 출력: "2 1"
  EXPECT_EQ("2 1", GetTrimmedOutput([&] { s.Rank(5); }));

  // 중간값 (Key: 10) -> Rank 2
  // 20의 왼쪽 자식 (Depth 1, Height 2) -> 1 * 2 = 2
  // 출력: "2 2"
  EXPECT_EQ("2 2", GetTrimmedOutput([&] { s.Rank(10); }));

  //루트값 (Key: 20) -> Rank 4 (왼쪽 자식들 3개 + 자신 1)
  // Root (Depth 0, Height 3) -> 0 * 3 = 0
  // 출력: "0 4"
  EXPECT_EQ("0 4", GetTrimmedOutput([&] { s.Rank(20); }));

  // 최대값 (Key: 40) -> Rank 7
  // Leaf (Depth 2, Height 1) -> 2 * 1 = 2
  // 출력: "2 7"
  EXPECT_EQ("2 7", GetTrimmedOutput([&] { s.Rank(40); }));

  // 우측 서브트리 내부 값 (Key: 25) -> Rank 5
  // 위치: Leaf (Depth 2, Height 1) -> 2 * 1 = 2
  // 출력: "2 5"
  EXPECT_EQ("2 5", GetTrimmedOutput([&] { s.Rank(25); }));
}

TEST_F(PrevNextTest, Rank_NotFound) {
  // 존재하지 않는 값 검색

  // 트리 범위 내에 없는 값
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.Rank(100); }));
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.Rank(0); }));

  // 트리 범위 사이의 없는 값
  EXPECT_EQ("-1", GetTrimmedOutput([&] { s.Rank(12); }));
}

TEST_F(AVLSetTest, Rank_EmptyTree) {
  // 빈 트리에서 Rank 호출
  string out = CaptureStdout([&] { s.Rank(10); });
  EXPECT_EQ("-1", OneToken(out));
}

// -------------------------Erase 테스트--------------------------

// 존재하지 않는 노드 삭제
TEST_F(AVLSetTest, Erase_NotFound) {
  CaptureStdout([&] { s.Insert(10); });

  // 없는 노드 삭제 시 -1 출력
  EXPECT_EQ("-1", OneToken(CaptureStdout([&] { s.Erase(20); })));

  // 데이터 개수 유지 확인
  EXPECT_EQ(s.n_, 1);
}

// 2. Leaf 노드 삭제
TEST_F(PrevNextTest, Erase_Leaf) {
  EXPECT_EQ("2", OneToken(CaptureStdout([&] { s.Erase(5); })));

  EXPECT_EQ(s.n_, 6);
  EXPECT_EQ(s.FindNode(5), nullptr); // 노드가 사라졌는지 확인

  // 부모(10)의 연결 끊김 확인
  Node *n10 = s.FindNode(10);
  EXPECT_EQ(n10->left, nullptr);

  // 부모의 높이, 사이즈 갱신 확인 (10은 Leaf)
  EXPECT_EQ(n10->height, 2);
  EXPECT_EQ(n10->size, 2);
}

// 자식이 1개인 노드 삭제
TEST_F(AVLSetTest, Erase_OneChild) {
  // 트리 구성: 10(Root) -> 5(Left)
  CaptureStdout([&] {
    s.Insert(10);
    s.Insert(5);
  });

  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Erase(10); })));

  // 5가 새로운 루트가 되어야 함
  EXPECT_EQ(s.n_, 1);
  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, 5);
  EXPECT_EQ(s.root_->parent, nullptr);
  EXPECT_EQ(s.root_->height, 1);
}

// 자식이 2개인 노드 삭제
TEST_F(PrevNextTest, Erase_TwoChildren_Root) {
  // 20 삭제 -> 후임자인 25가 20의 자리로 이동
  EXPECT_EQ("0", OneToken(CaptureStdout([&] { s.Erase(20); })));

  EXPECT_EQ(s.n_, 6);

  // 루트가 25로 변경
  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, 25);

  // 기존 20의 자식들(10, 30)을 입양
  EXPECT_EQ(s.root_->left->key, 10);
  EXPECT_EQ(s.root_->right->key, 30);
  EXPECT_EQ(s.root_->left->parent, s.root_);
  EXPECT_EQ(s.root_->right->parent, s.root_);

  // 25가 원래 있던 자리가 비었는지
  // 25가 올라갔으니 30의 왼쪽은 nullptr
  Node *n30 = s.FindNode(30);
  EXPECT_EQ(n30->left, nullptr);

  // 높이 갱신 확인
  // 30은 자식이 40(Right)만 남음 -> Height 2
  EXPECT_EQ(n30->height, 2);
  // New Root(25) -> Left(10, H=2) + Right(30, H=2) -> Height 3
  EXPECT_EQ(s.root_->height, 3);
}

// 삭제, 불균형 발생 및 회전 테스트
TEST_F(AVLSetTest, Erase_TriggerRotation) {

  CaptureStdout([&] {
    s.Insert(30);
    s.Insert(20);
    s.Insert(40);
    s.Insert(10);
  });

  // 40의 정보: Depth 1, Height 1 -> Output: 1
  EXPECT_EQ("1", OneToken(CaptureStdout([&] { s.Erase(40); })));

  EXPECT_EQ(s.n_, 3);

  // Root 확인 (20)
  ASSERT_NE(s.root_, nullptr);
  EXPECT_EQ(s.root_->key, 20);
  EXPECT_EQ(s.root_->parent, nullptr);
  EXPECT_EQ(s.root_->height, 2);

  // 자식 확인
  EXPECT_EQ(s.root_->left->key, 10);
  EXPECT_EQ(s.root_->right->key, 30);

  // 부모 연결 확인
  EXPECT_EQ(s.root_->left->parent, s.root_);
  EXPECT_EQ(s.root_->right->parent, s.root_);
}