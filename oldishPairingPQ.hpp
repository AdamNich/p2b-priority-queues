// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>

#include "Eecs281PQ.hpp"
using namespace std;

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        explicit Node(const TYPE &val, Node* c, Node* s, Node* p)
            : elt { val }, child(c), sibling(s), prev(p) {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        // TODO: Add and initialize one extra pointer (parent or previous) as desired.
        Node *prev;
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, n(0), root(nullptr) {
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, n(0), root(nullptr) {
        // TODO: Implement this function.
        if (start == end) return;
        // root = new Node(*start, nullptr, nullptr, nullptr);
        while (start != end) push(*(start++));
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare }, n(0), root(nullptr) {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        if (other.empty()) return;
        // root = new Node(other.root->elt, nullptr, nullptr, nullptr);
        deque<Node*> dq;
        dq.push_back(other.root);
        while (!dq.empty()) {
            //maybe .front()? is it better?
            if (dq.front()->sibling != nullptr) {
                dq.push_back(dq.front()->sibling);
            }
            if (dq.front()->child != nullptr) {
                dq.push_back(dq.front()->child);
            }
            push(dq.front()->elt);
            dq.pop_front();
        }
    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        PairingPQ temp(rhs);
        std::swap(n, temp.n);
        std::swap(root, temp.root);
        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() { 
        // TODO: Implement this function.
        deque<Node*> dq;
        if (root == nullptr) return;
        dq.push_back(root);
        while (!dq.empty()) {
            //maybe .front()? is it better?
            if (dq.front()->sibling != nullptr) {
                dq.push_back(dq.front()->sibling);
                dq.front()->sibling = nullptr;
            }
            if (dq.back()->child != nullptr) {
                dq.push_back(dq.back()->child);
                dq.front()->child = nullptr;
            }
            delete dq.front();
            dq.pop_front();
        }
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        if (n == 0 || n == 1) return;
        deque<Node*> dq;
        dq.push_back(root->child);
        root->child = nullptr;
        while (!dq.empty()) {
            if (dq.front()->child != nullptr) {
                dq.push_back(dq.front()->child);
                dq.front()->child = nullptr;
            }
            if (dq.front()->sibling != nullptr) {
                dq.push_back(dq.front()->sibling);
                dq.front()->sibling = nullptr;
            }
            dq.front()->prev = nullptr;
            root = meld(dq.front(), root);
            dq.pop_front();
        }
    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        --n;
        deque<Node*> dq;
        dq.push_back(root->child);
        delete root;
        //just in case the root has no children, so the root becomes nullptr
        root = dq[0];
        if (dq[0] == nullptr) return;
        // dq[0]->parent = nullptr;
        Node *sibling = dq.back()->sibling;
        while (sibling != nullptr) {
            dq.push_back(sibling);
            sibling = dq.back()->sibling;
            dq.back()->sibling = nullptr;
            dq.back()->prev = nullptr;
        }
        while (dq.size() > 1) {
            dq.push_back(meld(dq[0], dq[1]));
            dq.pop_front();
            dq.pop_front();
        }
        root = dq[0];
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        // TODO: Implement this function
        return n;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        // TODO: Implement this function
        return n == 0;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        if (node == root) return;
        if (node->prev->child == node) {
            node->prev->child = node->sibling;
            if (node->sibling != nullptr) node->sibling->prev = node->prev;
        } else {
            node->prev->sibling = node->sibling;
        }
        node->sibling = node->prev = nullptr;
        root = meld(node, root);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        // TODO: Implement this function
        ++n;
        Node *a = new Node(val, nullptr, nullptr, nullptr);
        if (n == 1) root = a;
        else root = meld(a, root);
        return a;
    }  // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).

    size_t n;
    Node *root;

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    //pa & pb have no parent & no previous  
    Node *meld(Node *pa, Node *pb) {
        if (this->compare(pa->elt, pb->elt)) {
            pa->sibling = pb->child;
            if (pb->child != nullptr) pb->child->prev = pa;
            pa->prev = pb;
            pb->child = pa;
            return pb;
        } else {
            pb->sibling = pa->child;
            if (pa->child != nullptr) pa->child->prev = pb;
            pb->prev = pa;
            pa->child = pb;
            return pa;
        }
    }
};


#endif  // PAIRINGPQ_H
