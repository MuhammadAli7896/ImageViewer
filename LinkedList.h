template <class T>
class Node
{
public:
    Node *prev;
    T data;
    Node *next;

    Node(T d)
    {
        prev = nullptr;
        data = d;
        next = nullptr;
    }
};

template <class T>
class LinkedList
{
public:
    Node<T> *head;
    Node<T> *tail; // keeps the track of the last node
    int length;

    LinkedList()
    {
        head = nullptr;
        tail = nullptr;
        length = 0;
    }
    // insert a node at the start of the linked list

    void prepend(T d)
    {
        if (head == nullptr)
        {
            Node<T> *newNode = new Node<T>(d);
            newNode->next = head;
            head = newNode;
            tail = newNode;
            newNode->next = head;
            head->prev = tail;
            length++;
        }
        else
        {
            Node<T> *newNode = new Node<T>(d);
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
            tail->next = head;
            length++;
        }
    }

    // insert a node at the end of the linked list
    void append(T d)
    {
        if (head == nullptr)
        {
            Node<T>* newNode = new Node<T>(d);
            newNode->next = head;
            head = newNode;
            tail = newNode;
            newNode->next = head;
            head->prev = tail;
            length++;
        }
        else
        {
            Node<T> *newNode = new Node<T>(d);
            newNode->prev = tail;
            tail->next = newNode;
            newNode->next = head;
            tail = newNode;
            head->prev = tail;
            length++;
        }
    }

    void deleteNode(T val)
    {

        Node<T> *tempptr = head;

        if (head == nullptr)
        {
            return;
        }
        // first node
        if (tempptr->data == val)
        {
            head = tempptr->next;
            tail->next = head;
            delete tempptr;
            return;
        }

        while (tempptr->next != nullptr && tempptr->next->data != val)
        {
            tempptr = tempptr->next;
            if (tempptr->next == nullptr)
            {
                return;
            }
        }

        Node<T> *todelete = tempptr->next;
        tempptr->next = todelete->next;
        // for last node
        if (todelete->next == head)
        {
            tempptr->next = head;
            tail = tempptr;
        }
        else
        {
            todelete->next->prev = tempptr;
        }

        delete todelete;
        length--;
    }
    ~LinkedList()
    {
        Node<T> *temp = head;
        while(temp != tail)
        {
            Node<T> *todelete = temp;
            temp = temp->next;
            delete todelete;
        }
        delete tail, head;
    }
};