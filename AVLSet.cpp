#include <iostream>
using namespace std;

class AVLSet {
public:
    AVLSet() : root(nullptr), n_(0) {}
    //기본기능
    void Find(int x);       // set에서 key == x 인 노드를 찾는다
    void Insert(int x);     // set에 존재하지 않는 새로운 키 x를 삽입한다
    void Empty();           // set이 비었는지 확인
    void Size();            // set에 저장된 원소의 개수 출력
    void Prev(int x);       // x보다 작은 값들중 가장 큰 원소 y를 찾는다
    void Next(int x);       // x보다 큰 값들중 가장 작은 원소 y를 찾는다
    void UpperBound(int x); // key가 k보다 큰 값들중 가장 작은 원소 y를 찾는다

    //고급기능
    void Rank(int x);  // 노드의 순위를 구한다
    void Erase(int x); // 노드 x를 삭제한다

private:
    struct Node;
    Node* root;
    int n_; // set의 크기

    // 기본 기능 구현 위한 함수들
    int BalanceDegree(Node* x);   // 균형 깨진 정도 측정
    void ResizeHs(Node* x);       // x노드의 height, size 재측정

    void ReBalance(Node* start);  //균형 맞추기
    Node* RotateLeft(Node* x);    //좌측으로 회전
    Node* RotateRight(Node* y);   //우측으로 회전

    Node* FindNode(int x);        //노드 반환
};

struct AVLSet::Node {
    Node(int k, Node* p = nullptr)
        : key(k), height(1), size(1), left(nullptr), right(nullptr), parent(p) {}
    int key;
    int height;
    int size; // 해당 노드를 루트로 하는 부분트리에 포함된 노드의 개수
    Node* left, * right, * parent;
};

int AVLSet::BalanceDegree(Node *x) {
    if (!x) {
        return 0;
    }
    int lh = x->left ? x->left->height : 0;
    int rh = x->right ? x->right->height : 0;
    return lh - rh;
}

void AVLSet::ResizeHs(Node *x) {
    if (!x) {
        return;
    }
    int lh = x->left ? x->left->height : 0;   // x의 왼쪽 자식 높이
    int rh = x->right ? x->right->height : 0; // x의 오른쪽 자식 높이
    x->height = 1 + (lh > rh ? lh : rh);

    int ls = x->left ? x->left->size : 0;     // x의 왼쪽 자식 사이즈
    int rs = x->right ? x->right->size : 0;   // x의 오른쪽 자식 사이즈
    x->size = 1 + ls + rs;
}



AVLSet::Node *AVLSet::FindNode(int x) {
    Node *cur_Node = root;
    while (cur_Node != nullptr) {
        if (cur_Node->key == x) {
            return cur_Node;
        }
        if (cur_Node->key > x) { // move to left_child
            cur_Node = cur_Node->left;
        } else { // move to right_child
            cur_Node = cur_Node->right;
        }
    }
    return nullptr;
}
int main(void) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        AVLSet set;
        string command;

        int Q;
        cin >> Q;
        while (Q--) {
            cin >> command;
            int x;
            if (command == "Find") {
                if (cin >> x)
                    set.Find(x);
            }
            else if (command == "Insert") {
                if (cin >> x)
                    set.Insert(x);
            }
            else if (command == "Empty") {
                set.Empty();
            }
            else if (command == "Size") {
                set.Size();
            }
            else if (command == "Prev") {
                if (cin >> x)
                    set.Prev(x);
            }
            else if (command == "Next") {
                if (cin >> x)
                    set.Next(x);
            }
            else if (command == "UpperBound") {
                if (cin >> x)
                    set.UpperBound(x);
            }
            else if (command == "Rank") {
                if (cin >> x)
                    set.Rank(x);
            }
            else if (command == "Erase") {
                if (cin >> x)
                    set.Erase(x);
            }
        }
    }

    return 0;
}