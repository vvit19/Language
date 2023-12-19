#include "backend.h"

const char* const NIL = "NIL";
const int  NIL_LENGTH = 3;

static char* ReadNode (Node* node, char* buffer);

Node* GetTree (char* buffer)
{
    assert (buffer);

    Node* fict_node = CreateOpNode (nullptr, nullptr, NEXT_OP);
    fict_node->left = fict_node;
    ReadNode (fict_node, buffer);

    Node* main_node = fict_node->right;
    free (fict_node);

    return main_node;
}

static char* ReadNode (Node* node, char* buffer)
{
    assert (node);

    if (!buffer) return nullptr;

    if (*buffer == '\0') return nullptr;

    Node* cur_node  = nullptr;

    int i = 0;
    while (buffer[i] != '\0')
    {
        while (buffer[i++] != '{')
        {
            if (buffer[i] == '\0') break;
        }

        i = SkipSpaces (buffer, i);

        if (strncmp (NIL, &buffer[i], NIL_LENGTH) == 0)
        {
            i += NIL_LENGTH;
            break;
        }

        if (isdigit (buffer[i]))
        {
            char* end_ptr = &buffer[i];
            elem_t num = strtod (&buffer[i], &end_ptr);
            cur_node = CreateNumNode (nullptr, nullptr, num);

            while (&buffer[i] != end_ptr) i++;
            break;
        }
        else
        {
            for (size_t cnt = 0; cnt < sizeof (OperationsArray) / sizeof (const char*); cnt++)
            {
                int op_length = strlen (OperationsArray[cnt]);
                if (strncmp (OperationsArray[cnt], &buffer[i], op_length) == 0)
                {
                    cur_node = CreateOpNode (nullptr, nullptr, (KeyWords) cnt);
                    i += op_length;
                    break;
                }
            }

            if (cur_node != nullptr) break;

            i = SkipSpaces (buffer, i);

            char variable[MAX_VAR_LENGTH] = "";
            for (int cnt = 0; isalpha (buffer[i]) || buffer[i] == '_' || isdigit (buffer[i]); i++, cnt++)
            {
                variable[cnt] = buffer[i];
            }

            cur_node = CreateVarNode (nullptr, nullptr, variable);

            if (node->type == OP_T && (node->value.op == CALL || node->value.op == FUNC) && (!node->left)) cur_node->type = FUNC_NAME;

            break;
        }
    }

    if (cur_node == nullptr)
    {
        if (!node->left)
        {
            node->left = node;
            return ReadNode (node, &buffer[i]);
        }

        if (node->left == node)
            node->left = nullptr;

        node->right = nullptr;

        return &buffer[i];
    }

    if (node->left)
    {
        node->right = cur_node;

        if (node->left == node)
            node->left  = nullptr;

        return ReadNode (node->right, &buffer[i]);
    }

    node->left = cur_node;
    char* buff = ReadNode (node->left, &buffer[i]);
    return ReadNode (node, buff);
}
