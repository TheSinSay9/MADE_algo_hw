#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <string>

#include "Huffman.h"

typedef unsigned char byte;

class BitsReader {
    // Класс для побитового и побайтовго считывания
public:
    BitsReader (std::vector<byte> &buffer);

    bool ReadBit();
    byte ReadByte();
    // Функция для проверки считывается ли последний байт
    bool IsLastByte();
    // Функция для проверки завершено ли считывание
    bool IsEnd();
    // На каком бите в данном байте находится указатель считывания
    byte curr_bit_in_byte = 0;

private:
    std::vector<byte> buffer_;
    // Указатель на бит, который будет считан следующим
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
    // Стракт для вершин из которых составляется дерево Хаффмана

    // Является ли вершина листом
    bool is_terminal = false;
    // Символ, если вершина - лист
    byte symbol = ' ';
    // Суммарная частота всех символов листов, которые находятся в поддеревьях этой вершины
    int value = 0;
    // Указатели на сыновей
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    TreeNode* parent = nullptr;

    TreeNode (bool is_terminal_, byte symbol_, int value_) {
        is_terminal = is_terminal_;
        symbol = symbol_;
        value = value_;
    }

    ~TreeNode();

};

TreeNode::~TreeNode() {
    // Циклический обход дерева PostOrder для удаления всех вершин
    // Указатель на текущую вершину
    TreeNode* traversal_pointer = this;
    // Указатель traversal_pointer может обнулиться лишь тогда, когда мы попытаеся идти вверх по дереву от корня.
    while (traversal_pointer != nullptr) {
        // Спуск вниз пока есть сыновья
        if (traversal_pointer->left) {
            traversal_pointer = traversal_pointer->left;
        } else if (traversal_pointer->right) {
            traversal_pointer = traversal_pointer->right;
        } else {
            // Удаление вершины и переход к её родителю
            TreeNode* for_delete = traversal_pointer;
            traversal_pointer = traversal_pointer->parent;
            if (traversal_pointer && traversal_pointer->left && for_delete == traversal_pointer->left)
                traversal_pointer->left = nullptr;
            else if (traversal_pointer && traversal_pointer->right)
                traversal_pointer->right = nullptr;
            delete for_delete;
        }
    }
}

class Comparator {
    // Компаратор для создания приоритетной очереди из деревьев с символами высоты 1
public:
    bool operator () (TreeNode* a, TreeNode* b) {
        return a->value > b->value;
    }
};


TreeNode* build_tree (std::vector<byte> &message) {
    // Создание дерева для кодов Хаффмана

    // map для хранения частот byte-слов
    std::unordered_map<byte, int> occurences;

    // Получение частот слов
    for (auto it = message.begin(); it != message.end(); it++) {
        occurences[*it]++;
    }

    // Создание очереди с приоритетом из деревьев имеющих только корень
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, Comparator> queue;
    for (auto it = occurences.begin(); it!=occurences.end(); it++) {
        TreeNode* new_node = new TreeNode(true, it->first, it->second);
        queue.push(new_node);
    }

    // Сооставление дерева Хаффмана из полученных ранее деревьев
    while (queue.size() > 1) {
        TreeNode* first = queue.top();
        queue.pop();
        TreeNode* second = queue.top();
        queue.pop();
        TreeNode* new_node = new TreeNode(false, ' ', 0);
        new_node->left = first;
        new_node->right = second;
        new_node->value = first->value + second->value;
        first->parent = new_node;
        second->parent = new_node;
        queue.push(new_node);
    }
    TreeNode* root = queue.top();
    queue.pop();

    return root;
}

std::unordered_map<byte, std::vector<bool>> MakeTable(TreeNode* root) {
    // Создание таблицы кодов по построенному дереву

    /*
     * Алгоритм реализован с помощью циклического InOrder подхода и полностью аналогичен тому, что был применён в
     * в первом домашнем задании на бинарные деревья.
     */

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

    while (traversal_pointer != nullptr) {
        if (!back) {
            /*
             * При движении вниз проверяем есть ли левый сын, если есть, тоо идём в него, если нет, то добавляем в
             * таблицу сисвол, если вершина корневая. Если есть правый сын то идём в него, если нет, то идём обратно.
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
             * Если от левого то пробуем двигаться вправо вниз. Если справа сына нет, то продолжаем двигатсья вверх.
             * Если мы вернулись от правого сына, то продолжаем идти вверх.
             */
            if (traversal_pointer->left == back_pointer) {
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
    return std::move(table);
}

void CodeTree(TreeNode* root, BitsWriter &bits_writer) {
    /*
     * Функция для записи перед кодом Хаффмана код для самой таблицы
     * Реализация PreOrder обхода дерева в цикле ананлогично тому, что в методе build_tree.
    */

    // Указатель на текущую вершину
    TreeNode* traversal_pointer = root;
    // Указатель на вершину, из которой мы пришли снизу, при движении по дереву вверх
    TreeNode* back_pointer = nullptr;
    // Для определения того, движемся ли мы вверх (true) или вниз (false)
    bool back = false;

    while (traversal_pointer != nullptr) {
        if (!back) {
            /*
             * Если находимся в листе, то пишем сначал бит 1, далее байт символа, записанного в листе и идём обратно.
             * Если же это не лист, то пишем бит 0 и пытаемся идти вниз.
             */
            if (traversal_pointer->is_terminal) {
                bits_writer.WriteBit(true);
                bits_writer.WriteByte(traversal_pointer->symbol);
                back_pointer = traversal_pointer;
                traversal_pointer = traversal_pointer->parent;
                back = true;
            } else {
                bits_writer.WriteBit(false);
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
             * Если от левого то пробуем двигаться вправо вниз. Если справа сына нет, то продолжаем двигатсья вверх.
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
}

TreeNode* ReadTree (BitsReader &bits_reader) {
    // Создание дерева Хаффмана по начальной части кода.

    // Структура кода 0 - вершина не листовая, 1 - вершина листовая + байт далее - симвоол в этой вершине.

    // Считывание первого бита и проверка на то, листовая ли это вершина
    bool first_bit = bits_reader.ReadBit();
    if (first_bit) {
        byte symbol = bits_reader.ReadByte();
        TreeNode *root = new TreeNode{true, symbol, 0};
        return root;
    } else {
        TreeNode* root = new TreeNode(false, ' ', 0);
        TreeNode* traversal_pointer = root;

        /*
         * Основная часть построения дерева. Создаётся корень и далее происходит обход в глубину с добавлением новых
         * вершин в качестве левых сыновей до того момента, когда левый сын не окажется листоом. Дальше после этого
         * обход в глубину продолжается аналогично в сторону правого сына. При достижении вершины, у которой оба
         * сына это листы происходит обход обратно без чтения следующего символа до тех пор, пока не найдётся новая
         * вершина, у которой нет правого или левого сына и чтение кода продолжается. Выход из цикла осуществляется,
         * если при проходе обратно не удаётся найти найти ни одну вершину, у которой нет обоих детей, т.е. когда
         * дерево полностью построено.
         */

        while (true) {
            bool bit = bits_reader.ReadBit();
            if (bit) {
                // Добавление листовой вершины как левого сына или правого с поиском новой вершины без правого сына
                byte symbol = bits_reader.ReadByte();
                TreeNode* new_node = new TreeNode(true, symbol, 0);
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
                // Добавление нелистовой вершины
                if (!traversal_pointer->left) {
                    TreeNode* new_node = new TreeNode(false, ' ', 0);
                    traversal_pointer->left = new_node;
                    new_node->parent = traversal_pointer;
                    traversal_pointer = new_node;
                } else {
                    TreeNode* new_node = new TreeNode(false, ' ', 0);
                    traversal_pointer->right = new_node;
                    new_node->parent = traversal_pointer;
                    traversal_pointer = new_node;
                }
            }
        }
    }
}

std::vector<byte> Decode_Internal(BitsReader &bits_reader, TreeNode* tree_root, int last_byte_bits_count) {
    // Декодирования сообщения с помощью дерева Хаффмана

    TreeNode* traversal_pointer = tree_root;
    std::vector<byte> output;

    // Отдельный вариант на случай, если сообщение состоит из одного символа
    if (tree_root->is_terminal) {
        output.push_back(tree_root->symbol);
        return output;
    }
    else {
        if (last_byte_bits_count == 0)
            last_byte_bits_count = 8;

        /*
         * Основной вариант. Считывание и проход до листа, после чего вывод символа и возвращение в корень до тех пор
         * пока сообщение не будет полностью прочитано.
        */
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

void Encode(IInputStream& original, IOutputStream& compressed) {
    // Функция для построения кода Хаффмана

    // Запись входного потока байт для кодирования
    std::vector<byte> message;
    byte h;
    while (original.Read(h)) {
        message.push_back(h);
    }

    // Создание дерева для кодирования
    TreeNode* root = build_tree(message);

    // Создание таблицы для кодирования по полученному дереву
    std::unordered_map<byte, std::vector<bool>> table = MakeTable(root);

    BitsWriter bits_writer;

    // Функция для записи кода дерева перед кодом самого сообщения
    CodeTree(root, bits_writer);

    // Кодирование самого сообщения с помощью таблицы
    for (auto it = message.begin(); it != message.end(); it++) {
        std::vector<bool> code = table[*it];
        for (auto in_it = code.begin(); in_it != code.end(); in_it++) {
            bits_writer.WriteBit(*in_it);
        }
    }

    // Получение итогового сообщения
    auto result = bits_writer.GetResult();

    // Запись итогового закодирванного сообщения в поток
    for (auto elem : result) {
        compressed.Write(elem);
    }
}

void Decode(IInputStream& compressed, IOutputStream& original) {
    // Функция для декодирования сообщения, закодированного функцией Encode

    // Считывание сообщения в вектор байт
    std::vector<byte> result;
    byte h;
    while(compressed.Read(h)){
        result.push_back(h);
    }
    // Количество бит в последнем байте
    int last_char_bits_count = result.back();
    result.pop_back();

    BitsReader bits_reader = BitsReader(result);
    // Считывание дерева Хаффмана
    TreeNode* tree = ReadTree(bits_reader);

    // Декодирование оставшейся части сообщения
    std::vector<byte> output = Decode_Internal(bits_reader, tree, last_char_bits_count);

    for (auto elem : output) {
        original.Write(elem);
    }
}

