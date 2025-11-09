#pragma once
#include <vector>
#include <functional>
#include <utility>
namespace transport {

    // KeyOf(T) debe devolver la clave ordenable (por defecto int para Station.id)
    template <typename T, typename KeyT = int>
    class BST {
        struct Node {
            T value;
            Node* left{ nullptr };
            Node* right{ nullptr };
            explicit Node(const T& v) : value(v) {}
        };
        Node* root_{ nullptr };
        std::function<KeyT(const T&)> keyOf_;

    public:
        explicit BST(std::function<KeyT(const T&)> keyOf) : keyOf_(std::move(keyOf)) {}
        ~BST() { clear(root_); }

        BST(const BST&) = delete;
        BST& operator=(const BST&) = delete;

        void insert(const T& v) { root_ = insertRec(root_, v); }
        const T* find(const KeyT& key) const { return findRec(root_, key); }
        bool erase(const KeyT& key) { bool erased = false; root_ = eraseRec(root_, key, erased); return erased; }

        std::vector<T> inOrder() const { std::vector<T> out; inOrderRec(root_, out); return out; }
        std::vector<T> preOrder() const { std::vector<T> out; preOrderRec(root_, out); return out; }
        std::vector<T> postOrder() const { std::vector<T> out; postOrderRec(root_, out); return out; }

    private:
        static void clear(Node* n) { if (!n) return; clear(n->left); clear(n->right); delete n; }

        Node* insertRec(Node* n, const T& v) {
            if (!n) return new Node(v);
            if (keyOf_(v) < keyOf_(n->value)) n->left = insertRec(n->left, v);
            else if (keyOf_(n->value) < keyOf_(v)) n->right = insertRec(n->right, v);
            else n->value = v; // replace on duplicate key
            return n;
        }

        const T* findRec(Node* n, const KeyT& k) const {
            if (!n) return nullptr;
            if (k < keyOf_(n->value)) return findRec(n->left, k);
            if (keyOf_(n->value) < k) return findRec(n->right, k);
            return &n->value;
        }

        Node* eraseRec(Node* n, const KeyT& k, bool& erased) {
            if (!n) return nullptr;
            if (k < keyOf_(n->value)) n->left = eraseRec(n->left, k, erased);
            else if (keyOf_(n->value) < k) n->right = eraseRec(n->right, k, erased);
            else {
                erased = true;
                if (!n->left) { auto r = n->right; delete n; return r; }
                if (!n->right) { auto l = n->left; delete n; return l; }
                // two children: promote successor
                Node* s = n->right;
                while (s->left) s = s->left;
                n->value = s->value;
                n->right = eraseRec(n->right, keyOf_(s->value), erased = true);
            }
            return n;
        }

        static void inOrderRec(Node* n, std::vector<T>& out) { if (!n) return; inOrderRec(n->left, out); out.push_back(n->value); inOrderRec(n->right, out); }
        static void preOrderRec(Node* n, std::vector<T>& out) { if (!n) return; out.push_back(n->value); preOrderRec(n->left, out); preOrderRec(n->right, out); }
        static void postOrderRec(Node* n, std::vector<T>& out) { if (!n) return; postOrderRec(n->left, out); postOrderRec(n->right, out); out.push_back(n->value); }
    };

} // namespace transport