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

AVLSet::Node *AVLSet::RotateLeft(Node *x) {
    if (!x || !x->right) {
        return x;
    }
    Node *y = x->right;
    Node *B = y->left;

    // x, y, B 재배치
    y->left = x;
    x->right = B;

    // parent 갱신
    y->parent = x->parent;
    if (B) {
        B->parent = x;
    }
    x->parent = y;

    // x가 x의 부모의 어디에서 왔는지에 따른 재배치
    if (!y->parent)
        root = y;
    else if (y->parent->left == x) {
        y->parent->left = y;
    } else {
        y->parent->right = y;
    }

    ResizeHs(x);
    ResizeHs(y);

    return y;
}

AVLSet::Node *AVLSet::RotateRight(Node *y) {
    if (!y || !y->left) {
        return y;
    }
    Node *x = y->left;
    Node *B = x->right;

    x->right = y;
    y->left = B;

    x->parent = y->parent;
    if (B)
        B->parent = y;
    y->parent = x;

    if (!x->parent)
        root = x;
    else if (x->parent->left == y) {
        x->parent->left = x;
    } else {
        x->parent->right = x;
    }

    ResizeHs(y);
    ResizeHs(x);

    return x;
}

void AVLSet::ReBalance(Node* start) {
    Node* cur = start;

    while (cur) {
        // 현재 노드의 height, size 재측정
        ResizeHs(cur);

        int balance = BalanceDegree(cur);

        // 왼쪽으로 기움 : LL or LR
        if (balance == 2) {
            // LR: 왼쪽 자식을 먼저 좌회전
            if (BalanceDegree(cur->left) < 0)
                RotateLeft(cur->left);

            RotateRight(cur);
        }

        // 오른쪽으로 기움 : RR or RL
        else if (balance == -2) {
            // RL: 오른쪽 자식을 먼저 우회전
            if (BalanceDegree(cur->right) > 0)
                RotateRight(cur->right);

            RotateLeft(cur);
        }

        // 부모로 올라가서 체크
        cur = cur->parent;
    }
}

AVLSet::Node *AVLSet::FindNode(int x) {
    Node *cur_Node = root;
    while (cur_Node != nullptr) {
        if (cur_Node->key == x) {
            return cur_Node;
        }
        if (cur_Node->key > x) { // 왼쪽 자식으로 이동
            cur_Node = cur_Node->left;
        } else { // 오른쪽 자식으로 이동
            cur_Node = cur_Node->right;
        }
    }
    return nullptr;
}

void AVLSet::Find(int x) {
    Node *cur_Node = root;
    int depth = 0;

    while (cur_Node != nullptr) {
        if (cur_Node->key == x) {
            cout << depth * cur_Node->height << '\n';
            return;
        }

        if (cur_Node->key > x) { // 왼쪽 자식으로 이동
            cur_Node = cur_Node->left;
        } else {
            cur_Node = cur_Node->right;
        }
        depth++;
    }
    
    cout << -1 << '\n'; // 찾지 못함
}

void AVLSet::Empty() {
    if (n_ == 0) {
        cout << 1 << '\n';
    } else {
        cout << 0 << '\n';
    }
}

void AVLSet::Size() {
    cout << n_ << '\n';
}

void AVLSet::Insert(int x) {
    Node* new_node = new Node(x);

    if (root == nullptr) { // 빈 트리일 경우
        root = new_node;
        ++n_;
        cout << 0 << '\n';
        return;
    }

    Node* p_node = nullptr;
    Node* cur_node = root;

    while (cur_node != nullptr) {
        p-node = cur_node;

        if (cur_node->key > x) { // 왼쪽 자식으로 이동
            cur_node = cur_node->left;
        }
        else { // 오른쪽 자식으로 이동
            cur_node = cur_node->right;
        }
    }

    new_node->parent = p_node;
    if (p_node->key > x) {
        p_node->left = new_node;
    }
    else {
        p_node->right = new_node;
    }

    ++n_;

    // 삽입 후 재정렬
    ReBalance(p_node);

    // 깊이 * 높이 출력
    Node* result_node = root;
    int depth = 0;
    while (result_node != nullptr && result_node->key != x) {
        if (result_node->key > x) {
            result_node = result_node->left;
        }
        else {
            result_node = result_node->right;
        }
        depth++;
    }

    cout << depth * result_node->height << '\n';
}

void AVLSet::Prev(int x) {
    Node* x_node = FindNode(x);
    Node* y_node = nullptr;

    if (x_node->left) { // 왼쪽 자식이 있는 경우
        y_node = x_node->left;
        while (y_node->right) {
            y_node = y_node->right;
        }
    }
    else { // 왼쪽 자식이 없는 경우
        Node* cur_node = x_node;
        Node* p_node = cur_node->parent;
        while (p_node && p_node->left == cur_node) {
            cur_node = p_node;
            p_node = p_node->parent;
        }
        y_node = p_node;
    }

    // y_node가 없는 경우
    if (y_node == nullptr) {
        cout << -1 << '\n';
        return;
    }

    // key값과 깊이 * 높이를 공백으로 구분하여 출력
    int depth = 0;
    for (Node* t = y_node; t && t->parent; t->parent) {
        depth++;
    }
    cout << y_node->key << ' ' << depth * y_node->height << '\n';

void AVLSet::Next(int x) {
    Node* x_node = FindNode(x);
    Node* y_node = nullptr;

    if (x_node->right) { // 오른쪽 자식이 있는 경우
        while (y_node->left) {
            y_node = y_node->left;
        }
    }
    else { // 오른쪽 자식이 없는 경우
        Node* cur_node = x_node;
        Node* p_node = cur->parent;
        while (p && p->right == cur) {
            cur_node = p_node;
            p_node = p_node->parent;
        }
        y_node = p;
    }
    // y_node가 없는 경우
    if (y_node == nullptr) {
        cout << -1 << '\n';
        return;
    }

    // key값과 깊이 * 높이를 공백으로 나눠서 출력
    int depth = 0;
    for (Node* t = y_node; t && t->parent; t = t->parnt) {
        depth++;
    }

    cout << y_node->key << ' ' << depth * y_node->height << '\n';
}


void AVLSet::UpperBound(int x) {
    Node *cur = root;
    Node *result_node = nullptr;

    while (cur) {
        if (cur->key > x) {
            result_node = cur;
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }

    if (!result_node) {
        cout << -1 << '\n';
        return;
    }

    int depth = 0;

    Node* t = result_node;
    while (t != nullptr) {
        if (t->parent != nullptr) {
            depth++;
            t = t->parent;
        } else {
            break;
        }
    }

    cout << result_node->key << ' ' << depth * result_node->height << '\n';
}

void AVLSet::Rank(int x) {
    Node* current = root; //root부터 내려가며 탐색
    int rank = 0;
    int depth = 0;

    while (current != nullptr) {
        if (x < current->key) {
            current = current->left;
            depth++;
        }
        else if (x > current->key) {
            int leftsize = (current->left != nullptr) ? current->left->size : 0; // 왼쪽 서브트리 크기
            rank += leftsize + 1;      // 왼쪽 + 현재 노드
            current = current->right;
            depth++;
        }
        else { // x == cur->key (찾음)
            int leftsize = (current->left != nullptr) ? current->left->size : 0;
            rank += leftsize + 1;
            cout << (depth * current->height) << ' ' << rank << '\n';
            return;
        }
    }

    cout << -1 << '\n'; // 못 찾은 경우
}


void AVLSet::Erase(int x) {
    Node* node = FindNode(x);
    if (node == nullptr) {
        cout << -1 << '\n';
        return;
    }


    int depth = 0;
    for (Node* t = node; (t != nullptr && t->parent != nullptr); t = t->parent)
        depth++;
    cout << depth * node->height << '\n'; //깊이*높이 출력


    Node* del = node; //삭제 대상 노드

    if ((node->left != nullptr) && (node->right != nullptr)) { //자식 2개일때
        Node* successor = node->right;
        while (successor->left != nullptr)
            successor = successor->left;

        Node* balancenode = (successor->parent);

        node->key = successor->key;
        del = successor;
        delete del;
        --n_;
        ReBalance(balancenode);
    }
    else { // del이 최대 한 자식(1 또는 0)
        Node* child = (del->left != nullptr) ? del->left : del->right; 
        Node* parent = del->parent;

        if (child != nullptr)
            child->parent = parent;
        if (parent == nullptr) {
            root = child;
        }
        else if (parent->left == del) {
            parent->left = child;
        }
        else {
            parent->right = child;
        }

        delete del;
        --n_;
        ReBalance(parent);
    }



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
