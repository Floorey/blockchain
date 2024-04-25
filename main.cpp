#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>


class Transaction {
private:
    std::string sender;
    std::string recipient;
    float amount;

public:
    Transaction(const std::string& from, const std::string& to, float amt)
        : sender(from), recipient(to), amount(amt) {}

    std::string getSender() const {
        return sender;
    }

    std::string getRecipient() const {
        return recipient;
    }

    float getAmount() const {
        return amount;
    }
};


class Block {
private:
    int index;
    std::string previousHash;
    std::vector<Transaction> transactions;
    std::string hash;
    time_t timestamp;
    int nonce;
    int difficulty;

public:
    Block(int idx, const std::string& prevHash, const std::vector<Transaction>& txns, int diff) 
        : index(idx), previousHash(prevHash), transactions(txns), difficulty(diff) {
        timestamp = std::time(nullptr);
        nonce = 0;
        hash = mineBlock();
    }

    std::string calculateHash(int nonce) const {
        std::stringstream ss;
        ss << index << previousHash << timestamp << nonce;
        for(const auto& txn : transactions) {
            ss << txn.getSender() << txn.getRecipient() << txn.getAmount();
        }
        return sha256(ss.str());
    }

    std::string mineBlock() {
        std::string target(difficulty, '0'); // Zielhash beginnt mit einer bestimmten Anzahl von Nullen, um die Schwierigkeit zu definieren
        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash(nonce);
        }
        std::cout << "Block mined: " << hash << std::endl;
        return hash;
    }

    std::string getHash() const {
        return hash;
    }

    std::string getPreviousHash() const {
        return previousHash;
    }

    void printBlock() const {
        std::cout << "Block #" << index << std::endl;
        std::cout << "Previous Hash: " << previousHash << std::endl;
        std::cout << "Timestamp: " << std::ctime(&timestamp);
        std::cout << "Transactions:" << std::endl;
        for (const Transaction& txn : transactions) {
            std::cout << "  Sender: " << txn.getSender() << ", Recipient: " << txn.getRecipient() << ", Amount: " << txn.getAmount() << std::endl;
        }
        std::cout << "Hash: " << hash << std::endl;
        std::cout << std::endl;
    }
    
private:
    std::string sha256(const std::string& input) const {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input.c_str(), input.length());
        SHA256_Final(hash, &sha256);
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
};

// Blockchain-Klasse, enthält eine Kette von Blöcken
class Blockchain {
private:
    std::vector<Block> chain;
    int difficulty;

public:
    Blockchain(int diff) : difficulty(diff) {
        // Genesis-Block erstellen
        chain.emplace_back(0, "0", std::vector<Transaction>{}, difficulty);
    }

    void addBlock(const std::vector<Transaction>& transactions) {
        Block lastBlock = chain.back();
        Block newBlock(chain.size(), lastBlock.getHash(), transactions, difficulty);
        chain.push_back(newBlock);
    }

    void printBlockchain() const {
        for (const Block& block : chain) {
            block.printBlock();
        }
    }
};

int main() {
    // Erstellen einer Blockchain mit Schwierigkeitsgrad 4 (nur als Beispiel)
    Blockchain myBlockchain(4);

    // Erstellen einer Transaktion
    std::vector<Transaction> transactions;
    transactions.push_back(Transaction("Sender", "Empfänger", 10.0)); // Beispieltransaktion hinzufügen

    // Hinzufügen eines Blocks mit Transaktionsdaten
    myBlockchain.addBlock(transactions);

    // Ausgabe der Blockchain
    myBlockchain.printBlockchain();

    return 0;
}
