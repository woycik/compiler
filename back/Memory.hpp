#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>



class MemoryItem {
public:
    long long address;
    std::string identifier;
    std::string parent;
    bool isReference;

    MemoryItem(long long address, std::string parent, std::string identifier, bool isReference)
        : address(address), parent(parent), identifier(identifier), isReference(isReference) {
    }
};

class Memory {
public:
    std::vector<MemoryItem> memoryArray;

    long long memoryPointer = 0;

    Memory();
    ~Memory();
    void insertToMemory(long long address, std::string parent, std::string identifier, bool isReference);
    long long getAddressForIdentifier(const std::string& parent, const std::string& identifier);
    void setMemoryItemAddress(const std::string& parent, const std::string& name, long long address);
    long long getMemoryPointer();
    void setMemoryItemName(const std::string& parent, const std::string& currentName, const std::string& newName);
    std::vector<std::string> getItemsByParent(const std::string& parent) const;
    bool isArgReference(const std::string& parent,const std::string& identifier);
    void setArgReference(const std::string& parent,const std::string& identifier, bool reference);

};




#endif // MEMORY_HPP
