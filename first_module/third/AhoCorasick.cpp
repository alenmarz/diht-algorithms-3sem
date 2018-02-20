#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <unordered_map>
#include <string>
#include <vector>

using std::deque;
using std::set;
using std::unordered_map;
using std::string;
using std::vector;

const char DONT_CARE_JOKER = '?';

typedef unsigned int uint;


class TrieNode {
private:
    char symbol_;
    unordered_map<char, TrieNode *> children_;
    TrieNode * rollback_node_;
    std::vector<uint> pattern_entries_;

public:
    explicit TrieNode(char symbol)
        : symbol_(symbol),
          rollback_node_(NULL)
    {
    }
    
    char get_symbol() const {
        return symbol_;
    }

    unordered_map<char, TrieNode *> & get_children() {
        return children_;
    }

    const unordered_map<char, TrieNode *> & get_children() const {
        return children_;
    }

    TrieNode * find_child(char symbol) const {
        unordered_map<char, TrieNode *>::const_iterator child = children_.find(symbol);

        if (child != children_.end()) {
            return child->second;
        }

        return nullptr;
    }

    void add_child(TrieNode* child) {   
        children_[child->get_symbol()] = child;
    }

    vector<uint> & get_pattern_entries() {
        return pattern_entries_;
    }

    const vector<uint> & get_pattern_entries() const {
        return pattern_entries_;
    }

    void add_pattern_end(uint pattern_index) {
        if (
            std::find(
                pattern_entries_.begin(), pattern_entries_.end(),
                pattern_index
            ) == pattern_entries_.end()
        ) {
            pattern_entries_.push_back(pattern_index);
        }
    }

    void join_pattern_entries(TrieNode * node) {
        vector<uint> & pattern_entries = node->get_pattern_entries();
        for (size_t index = 0; index < pattern_entries.size(); ++index) {
            if (
                std::find(
                    pattern_entries_.begin(), pattern_entries_.end(),
                    pattern_entries[index]
                ) == pattern_entries_.end()
            ) {
                pattern_entries_.push_back(pattern_entries[index]);
            }
        }
    }

    TrieNode * get_rollback_node() const {
        return rollback_node_;
    }

    void set_rollback_node(TrieNode * node) {
        rollback_node_ = node;
    }
};


struct Subpattern {
    string line;
    uint position_in_pattern;
    uint length;

    Subpattern(const string & subpattern)
        : line(subpattern),
          position_in_pattern(0),
          length(line.length())
    {
    }

    void clear() {
        line.clear();
        length = 0;
    }

    bool empty() const {
        return length == 0;
    }

    char & operator [](size_t index) {
        return line[index];
    }

    char operator [](size_t index) const {
        return line[index];
    }

    void operator +=(char symbol) {
        line += symbol;
        ++length;
    }

    bool operator ==(const string & another) const {
        return line == another;
    }
};


class EntryProcessor {
public:
    EntryProcessor(
        const vector<Subpattern> & subpatterns,
        uint stripped_text_length
    ) : subpatterns_(subpatterns),
        occurrences_counters_(stripped_text_length, 0)
    {
    }

    void update(uint entry_position, uint pattern_number) {
        const Subpattern & subpattern = subpatterns_[pattern_number];

        uint position_to_update = 
            entry_position - subpattern.length - subpattern.position_in_pattern + 1;

        if (entry_position >= position_to_update) {
            ++occurrences_counters_[position_to_update];
        }
    }

    void finish() {
        uint subpatterns_size = subpatterns_.size();
        uint occurrences_size = occurrences_counters_.size();

        for (size_t index = 0; index < occurrences_size; ++index) {
            if (occurrences_counters_[index] == subpatterns_size) {
                std::cout << index << ' ';
            }
        }
    }

private:
    vector<Subpattern> subpatterns_;
    vector<uint> occurrences_counters_;
};


class Trie {
private:
    TrieNode * root_;
    TrieNode * find_rollback_node(TrieNode * parent, char symbol);
    void create_rollback_links();
    void update_entries(
        TrieNode * node, uint position_in_text, EntryProcessor & processor
    );

    void delete_trie(TrieNode * node);

public:
    Trie()
        : root_(new TrieNode(0))
    {
    }

    void construct(const vector<string> & patterns);
    void find_patterns_in_text(const string & text, EntryProcessor & processor);

    ~Trie() {
        delete_trie(root_);
    }
};


TrieNode * Trie::find_rollback_node(TrieNode * parent, char symbol) {
    TrieNode * node = parent;
    node = node->get_rollback_node();

    while (!node->find_child(symbol) && node != node->get_rollback_node()) {
        node = node->get_rollback_node();
    }

    if (node->find_child(symbol)) {
        return node->find_child(symbol);
    } else {
        return node;
    }
}


void Trie::create_rollback_links() {
    deque<TrieNode *> queue;
    // only root rollbacks to itself
    root_->set_rollback_node(root_);
    const unordered_map<char, TrieNode *> & children = root_->get_children();
    TrieNode * child;

    for (
        unordered_map<char, TrieNode *>::const_iterator it = children.begin();
        it != children.end(); ++it
    ) {
        child = it->second;
        child->set_rollback_node(root_);
        queue.push_front(child);
    }

    while (!queue.empty()) {
        TrieNode * current_node = queue.back();
        queue.pop_back();

        unordered_map<char, TrieNode *> & children = current_node->get_children();

        for (
            unordered_map<char, TrieNode *>::const_iterator it = children.begin();
            it != children.end(); ++it
        ) {

            TrieNode * child = it->second;
            queue.push_front(child);

            TrieNode * rollback_node = 
                find_rollback_node(current_node, child->get_symbol());

            child->set_rollback_node(rollback_node);
            child->join_pattern_entries(rollback_node);
        }
    } 
}


void Trie::update_entries(
    TrieNode * node, uint position_in_text, EntryProcessor & processor
) {
    const vector<uint> & pattern_entries = node->get_pattern_entries();
    uint pattern_entries_size = pattern_entries.size();

    for (size_t index = 0; index < pattern_entries_size; ++index) {
        processor.update(position_in_text, pattern_entries[index]);
    }
}


void Trie::delete_trie(TrieNode * node) {
    unordered_map<char, TrieNode *> & children = node->get_children();

    for (
        unordered_map<char, TrieNode *>::iterator it = children.begin();
        it != children.end(); ++it
    ) {
        delete_trie(it->second);
    }  

    delete node;
}

void Trie::construct(const vector<string> & patterns) {
    for (
        size_t pattern_index = 0; pattern_index < patterns.size();
        ++pattern_index
    ) {

        TrieNode * current_node = root_;
        string pattern(patterns[pattern_index]);

        for (size_t index = 0; index < pattern.length(); ++index) {
            if (current_node->find_child(pattern[index])) {
                current_node = current_node->find_child(pattern[index]);
            } else {
                TrieNode * new_node = new TrieNode(pattern[index]);
                current_node->add_child(new_node);
                current_node = new_node;
            }
        }

        current_node->add_pattern_end(pattern_index);
    }

    create_rollback_links();
}


void Trie::find_patterns_in_text(
    const string & text, EntryProcessor & processor
) {
    TrieNode * current_node = root_;
    TrieNode * child;
    char symbol;

    for (size_t position = 0; position < text.length(); ++position) {
        symbol = text[position];
        child = current_node->find_child(symbol);

        if (child != nullptr) {
            current_node = child;
        } else {
            do {
                current_node = current_node->get_rollback_node();
                child = current_node->find_child(symbol);
            } while (child == nullptr && current_node != root_);

            if (child != nullptr) {
                current_node = child;
            }
        }

        update_entries(current_node, position, processor);
    }
}


void read_data(string * pattern, string * text) {
    std::cin >> *pattern;
    std::cin >> *text;
}


void split_by_jokers(const string & pattern, vector<Subpattern> * subpatterns) {
    Subpattern part("");

    for (size_t index = 0; index < pattern.length(); ++index) {
        if (part == "" && pattern[index] != DONT_CARE_JOKER) {
            part.position_in_pattern = index;
        }

        if (pattern[index] != DONT_CARE_JOKER) {
            part += pattern[index];
        } else if (!part.empty()) {
            subpatterns->push_back(part);
            part.clear();
        }
    }

    if (!part.empty()) {
        subpatterns->push_back(part);
    }
}


void strip_jokers(string * pattern, string * text, uint * cut_from_begin) {
    *cut_from_begin = 0;
    uint cut_from_end = 0;

    while (
        *cut_from_begin < pattern->length() && 
        (*pattern)[*cut_from_begin] == DONT_CARE_JOKER
    ) {
        ++(*cut_from_begin);
    }

    if (*cut_from_begin == pattern->length()) {
        *pattern = "";
        return;
    }

    while ((*pattern)[pattern->length() - cut_from_end - 1] == DONT_CARE_JOKER) {
        ++cut_from_end;
    }

    uint pattern_count = pattern->length() - cut_from_end - *cut_from_begin;
    uint text_count = text->length() - cut_from_end - *cut_from_begin;

    *pattern = pattern->substr(*cut_from_begin, pattern_count);
    *text = text->substr(*cut_from_begin, text_count);
}


void find_pattern_in_text(const string & pattern, const string & text) {
    if (pattern.length() == 0) {
        for (size_t i = 0; i < text.length(); ++i) {
            std::cout << i << ' ';
        }
        return;
    }

    if (pattern.length() > text.length()) {
        return;
    }

    uint cut_from_begin;

    string stripped_pattern = pattern;
    string stripped_text = text;
    strip_jokers(&stripped_pattern, &stripped_text, &cut_from_begin);

    if (stripped_pattern.length() == 0) {
        for (size_t i = 0; i < text.length() - cut_from_begin + 1; ++i) {
            std::cout << i << ' ';
        }
        return;
    }

    vector<Subpattern> subpatterns;
    split_by_jokers(stripped_pattern, &subpatterns);

    vector<string> patterns(subpatterns.size());
    for (size_t index = 0; index < patterns.size(); ++index) {
        patterns[index] = subpatterns[index].line;
    }

    Trie trie;
    trie.construct(patterns);

    EntryProcessor processor(subpatterns, stripped_text.length());
    trie.find_patterns_in_text(stripped_text, processor);
    processor.finish();
}


int main() {
    std::ios_base::sync_with_stdio(false);

    string pattern, text;
    read_data(&pattern, &text);

    find_pattern_in_text(pattern, text);

    return 0;
}
