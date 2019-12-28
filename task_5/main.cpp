#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <string>

#include "Huffman.h"

typedef unsigned char byte;

class BitsReader {
public:
    BitsReader (std::vector<byte> &buffer);

    bool ReadBit();
    byte ReadByte();
    bool IsLastByte();
    bool IsEnd();
    byte curr_bit_in_byte = 0;

private:
    std::vector<byte> buffer_;
    std::vector<byte>::iterator iter;
};

BitsReader::BitsReader(std::vector<byte> &buffer) {
    buffer_ = buffer;
    iter = buffer_.begin();
}

bool BitsReader::IsLastByte() {
    int diff = buffer_.end() - iter;
    return diff <= 1;
}

bool BitsReader::IsEnd() {
    return iter == buffer_.end();
}

bool BitsReader::ReadBit() {
    byte curr_byte = *iter;
    unsigned char byte = *iter;
    bool bit = (byte >> curr_bit_in_byte) & 1;
    if (++curr_bit_in_byte == 8) {
        curr_bit_in_byte = 0;
        iter++;
    }
    return bit;
}

byte BitsReader::ReadByte() {
    byte curr_byte = *iter;
    byte read_byte = 0;
    if (curr_bit_in_byte == 0) {
        read_byte = curr_byte;
        iter++;
    } else {
        byte first_mask = (1 << (8 - curr_bit_in_byte)) - 1;
        read_byte = ((first_mask << curr_bit_in_byte) & (curr_byte)) >> curr_bit_in_byte;
        iter++;
        curr_byte = *iter;
        byte second_mask = (1 << curr_bit_in_byte) - 1;
        read_byte |= (second_mask & curr_byte) << (8 - curr_bit_in_byte);
    }
    return read_byte;
}

class BitsWriter {
public:
    void WriteBit(bool bit);

    void WriteByte(byte byte);

    std::vector<byte> GetResult();

private:
    std::vector<byte> buffer_;
    byte accumulator_ = 0;
    int bits_count_ = 0;
};

void BitsWriter::WriteBit(bool bit) {
    // Ставим бит в аккумулятор на нужное место
    accumulator_ |= static_cast<byte>(bit) << bits_count_;
    ++bits_count_;
    if (bits_count_ == 8) {
        bits_count_ = 0;
        buffer_.push_back(accumulator_);
        accumulator_ = 0;
    }
}

void BitsWriter::WriteByte(byte curr_byte) {
    if (bits_count_ == 0) {
        buffer_.push_back(curr_byte);
    } else {
        accumulator_ |= curr_byte << bits_count_;
        buffer_.push_back(accumulator_);
        accumulator_ = curr_byte >> (8 - bits_count_);
    }
}

std::vector<byte> BitsWriter::GetResult() {
    if (bits_count_ != 0) {
        // Добавляем в буфер аккумулятор, если в нем что-то есть.
        buffer_.push_back(accumulator_);
    }
    buffer_.push_back(static_cast<byte>(bits_count_));
    return std::move(buffer_);
}

struct TreeNode {
    bool is_terminal = false;
    byte symbol = ' ';
    int value = 0;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    TreeNode* parent = nullptr;

    TreeNode (bool is_terminal_, byte symbol_, int value_) {
        is_terminal = is_terminal_;
        symbol = symbol_;
        value = value_;
    }

    bool operator<(const TreeNode& b) const
    {
        return value > b.value;
    }
};

class Comparator {
public:
    bool operator () (TreeNode* a, TreeNode* b) {
        return a->value > b->value;
    }
};


TreeNode* build_tree (std::vector<byte> &message) {
    std::unordered_map<byte, int> occurences;

    for (auto it = message.begin(); it != message.end(); it++) {
        occurences[*it]++;
    }
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, Comparator> queue;
    for (auto it = occurences.begin(); it!=occurences.end(); it++) {
        TreeNode* new_node = new TreeNode(true, it->first, it->second);
        queue.push(new_node);
    }

    while (queue.size() > 1) {
        TreeNode* first = queue.top();
        queue.pop();
        TreeNode* second = queue.top();
        queue.pop();
        TreeNode* new_node = new TreeNode(false, ' ', 0);
        //std::cout << first->value << " ";
        //std::cout << second->value << std::endl;
        new_node->left = first;
        new_node->right = second;
        new_node->value = first->value + second->value;
        //std::cout << new_node->value << std::endl;
        first->parent = new_node;
        second->parent = new_node;
        queue.push(new_node);
    }
    TreeNode* root = queue.top();
    queue.pop();

    return root;
}

std::unordered_map<byte, std::vector<bool>> MakeTable(TreeNode* root) {
    // Указатель на текущую вершину
    TreeNode* traversal_pointer = root;
    // Указатель на вершину, из которой мы пришли снизу, при движении по дереву вверх
    TreeNode* back_pointer = nullptr;
    // Для определения того, движемся ли мы вверх (true) или вниз (false)
    bool back = false;
    std::unordered_map<byte, std::vector<bool>> table;
    std::vector<bool> current_code;
    if (root->is_terminal) {
        current_code.push_back(1);
        table[root->symbol] = current_code;
    }
    /*
     * Проход дерево InOrder в цикле. Указатель traversal_pointer в случае движения вниз по дереву никогда не может
     * стать нулевым, за счёт предварительных проверок на это. В случае же движеняи вверх traversal_pointer может стать
     * нулевым ровно тогда, когда указатель на родителя у вершины является нулевым, т.е. только тогда, когда мы
     * попытаемся подняться вверх по дереву, находясь в корне, что означает конец обхода.
     */
    while (traversal_pointer != nullptr) {
        if (!back) {
            /*
             * При движении вниз проверяем есть ли левый сын, если есть, тоо идём в него, если нет, то печатаем
             * значение вершины и пробуем идти вправо, если нет правого сына, то начинаем возвращаться по дереву вверх
            */
            if (traversal_pointer->left) {
                traversal_pointer = traversal_pointer->left;
                current_code.push_back(false);
            } else {
                if (traversal_pointer->is_terminal) {
                    table[traversal_pointer->symbol] = current_code;
                }
                if (traversal_pointer->right) {
                    traversal_pointer = traversal_pointer->right;
                    current_code.push_back(true);
                } else {
                    back_pointer = traversal_pointer;
                    traversal_pointer = traversal_pointer->parent;
                    current_code.pop_back();
                    back = true;
                }
            }
        } else {
            /*
             * При движении вверх проверяем, вернулись ли мы вверх от левеого сына или от правого.
             * Если от левого то записываем значение вершины и пробуем двигаться вправо вниз.
             * Если справа сына нет, то продолжаем двигатсья вверх.
             * Если мы вернулись от правого сына, то продолжаем идти вверх.
             */
            if (traversal_pointer->left == back_pointer) {
                if (traversal_pointer->is_terminal) {
                    table[traversal_pointer->symbol] = current_code;
                }
                if (traversal_pointer->right) {
                    traversal_pointer = traversal_pointer->right;
                    current_code.push_back(true);
                    back = false;
                } else {
                    back_pointer = traversal_pointer;
                    traversal_pointer = traversal_pointer->parent;
                    current_code.pop_back();
                }
            } else {
                back_pointer = traversal_pointer;
                traversal_pointer = traversal_pointer->parent;
                current_code.pop_back();
                back = true;
            }
        }
    }
    return table;
}

byte CodeTree(TreeNode* root, BitsWriter &bits_writer) {
    // Указатель на текущую вершину
    TreeNode* traversal_pointer = root;
    // Указатель на вершину, из которой мы пришли снизу, при движении по дереву вверх
    TreeNode* back_pointer = nullptr;
    // Для определения того, движемся ли мы вверх (true) или вниз (false)
    bool back = false;
    /*
     * Проход дерево PreOrder в цикле. Указатель traversal_pointer в случае движения вниз по дереву никогда не может
     * стать нулевым, за счёт предварительных проверок на это. В случае же движеняи вверх traversal_pointer может стать
     * нулевым ровно тогда, когда указатель на родителя у вершины является нулевым, т.е. только тогда, когда мы
     * попытаемся подняться вверх по дереву, находясь в корне, что означает конец обхода.
     */
    byte count = 0;
    while (traversal_pointer != nullptr) {
        if (!back) {
            /*
             * При движении вниз проверяем есть ли левый сын, если есть, тоо идём в него, если нет, то печатаем
             * значение вершины и пробуем идти вправо, если нет правого сына, то начинаем возвращаться по дереву вверх
            */
            if (traversal_pointer->is_terminal) {
                bits_writer.WriteBit(true);
                bits_writer.WriteByte(traversal_pointer->symbol);
                count += 9;
                back_pointer = traversal_pointer;
                traversal_pointer = traversal_pointer->parent;
                back = true;
            } else {
                bits_writer.WriteBit(false);
                count++;
                if (traversal_pointer->left) {
                    traversal_pointer = traversal_pointer->left;
                } else {
                    if (traversal_pointer->right) {
                        traversal_pointer = traversal_pointer->right;
                    } else {
                        back_pointer = traversal_pointer;
                        traversal_pointer = traversal_pointer->parent;
                        back = true;
                    }
                }
            }

        } else {
            /*
             * При движении вверх проверяем, вернулись ли мы вверх от левеого сына или от правого.
             * Если от левого то записываем значение вершины и пробуем двигаться вправо вниз.
             * Если справа сына нет, то продолжаем двигатсья вверх.
             * Если мы вернулись от правого сына, то продолжаем идти вверх.
             */
            if (traversal_pointer->left == back_pointer) {
                if (traversal_pointer->right) {
                    traversal_pointer = traversal_pointer->right;
                    back = false;
                } else {
                    back_pointer = traversal_pointer;
                    traversal_pointer = traversal_pointer->parent;
                }
            } else {
                back_pointer = traversal_pointer;
                traversal_pointer = traversal_pointer->parent;
                back = true;
            }
        }
    }
    return count;
}

TreeNode* ReadTree (BitsReader &bits_reader) {
    bool first_bit = bits_reader.ReadBit();
    if (first_bit) {
        byte symbol = bits_reader.ReadByte();
        TreeNode *root = new TreeNode{true, symbol, 0};
        return root;
    } else {
        TreeNode* root = new TreeNode(false, ' ', 0);
        TreeNode* traversal_pointer = root;
        int curr_bit = 1;
        while (true) {
            bool bit = bits_reader.ReadBit();
            if (bit) {
                byte symbol = bits_reader.ReadByte();
                TreeNode* new_node = new TreeNode(true, symbol, 0);
                curr_bit += 9;
                if (!traversal_pointer->left) {
                    traversal_pointer->left = new_node;
                    new_node->parent = traversal_pointer;
                } else {
                    traversal_pointer->right = new_node;
                    new_node->parent = traversal_pointer;
                    while (traversal_pointer -> right) {
                        if (traversal_pointer->parent)
                            traversal_pointer = traversal_pointer->parent;
                        else
                            return root;
                    }
                }
            } else {
                if (!traversal_pointer->left) {
                    TreeNode* new_node = new TreeNode(false, ' ', 0);
                    traversal_pointer->left = new_node;
                    new_node->parent = traversal_pointer;
                    traversal_pointer = new_node;
                    curr_bit++;
                } else {
                    TreeNode* new_node = new TreeNode(false, ' ', 0);
                    traversal_pointer->right = new_node;
                    new_node->parent = traversal_pointer;
                    traversal_pointer = new_node;
                    curr_bit++;
                }
            }
        }
    }
}

std::vector<byte> Decode_Internal(BitsReader &bits_reader, TreeNode* tree_root, int last_byte_bits_count) {
    TreeNode* traversal_pointer = tree_root;
    std::vector<byte> output;
    if (tree_root->is_terminal) {
        while (!bits_reader.IsLastByte() || (!bits_reader.IsEnd() && bits_reader.curr_bit_in_byte < last_byte_bits_count)) {
            bits_reader.ReadBit();
            output.push_back(tree_root->symbol);
        }
        return output;
    }
    else {
        if (last_byte_bits_count == 0)
            last_byte_bits_count = 8;
        while (!bits_reader.IsLastByte() || (!bits_reader.IsEnd() && bits_reader.curr_bit_in_byte < last_byte_bits_count)) {
            bool bit = bits_reader.ReadBit();
            if (bit) {
                traversal_pointer = traversal_pointer->right;
            } else {
                traversal_pointer = traversal_pointer->left;
            }
            if (traversal_pointer->is_terminal){
                output.push_back(traversal_pointer->symbol);
                traversal_pointer = tree_root;
            }
        }
        return output;
    }
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
    std::vector<byte> message;
    byte h;
    while (original.Read(h)) {
        message.push_back(h);
    }

    TreeNode* root = build_tree(message);

    auto table = MakeTable(root);

    BitsWriter bits_writer;

    byte count = CodeTree(root, bits_writer);

    for (auto it = message.begin(); it != message.end(); it++) {
        std::vector<bool> code = table[*it];
        for (auto in_it = code.begin(); in_it != code.end(); in_it++) {
            bits_writer.WriteBit(*in_it);
        }
    }
    auto result = bits_writer.GetResult();

    for (auto elem : result) {
        compressed.Write(elem);
    }
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
    std::vector<byte> result;
    byte h;
    while(compressed.Read(h)){
        result.push_back(h);
    }
    int last_char_bits_count = result.back();
    result.pop_back();

    BitsReader bits_reader = BitsReader(result);
    TreeNode* tree = ReadTree(bits_reader);

    std::vector<byte> output = Decode_Internal(bits_reader, tree, last_char_bits_count);

    for (auto elem : output) {
        original.Write(elem);
    }
}


