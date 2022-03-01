#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <bitset>

#include "Byte.h"

#define HUFFMAN_NOT_LEAF	((unsigned short)0x8000)

#pragma once

typedef struct _huffmantreewfreq {
	unsigned short value;
	unsigned int count;
	_huffmantreewfreq* left;
	_huffmantreewfreq* right;
} HuffmanFreqTree, *pHuffmanFreqTree;

typedef struct {
	unsigned int size;
	unsigned short data[1];
} HuffmanDict, * pHuffmanDict;

typedef struct {

} Huffman;


// priority_queue `Less` definition
struct LessHuffmanFreqTree {
	bool operator()(const HuffmanFreqTree* a, const HuffmanFreqTree* b) const {
		return a->count > b->count;
	}
};


static size_t huffmanFreqTreeNodeCount(const pHuffmanFreqTree root) {
	return root ? (1 + huffmanFreqTreeNodeCount(root->left) + huffmanFreqTreeNodeCount(root->right)) : 0;
}

// converts byte stream to huffman frequency tree
static pHuffmanFreqTree huffmanFreqTreeFromData(unsigned char* data, size_t dataLen) {
	if (!data || !dataLen) {
		std::runtime_error("Unable To Encode No Data");
	}

	std::priority_queue<
		pHuffmanFreqTree,				// data type 
		std::vector<pHuffmanFreqTree>,	// internal container type
		LessHuffmanFreqTree				// comparative function
	> freqTreeQueue;

	std::unordered_map<unsigned char, unsigned int> freqMap;

	for (size_t i = 0; i != dataLen; i++) {
		freqMap[data[i]]++;
	}

	for (const auto& freqEntry : freqMap) {
		std::cout << freqEntry.second << " : " << freqEntry.first << std::endl;
		freqTreeQueue.push(new HuffmanFreqTree {
			(unsigned short)freqEntry.first,
			freqEntry.second,
			NULL,
			NULL
		});
	}

	while (freqTreeQueue.size() != 1) {
		pHuffmanFreqTree left = freqTreeQueue.top();
		freqTreeQueue.pop();

		pHuffmanFreqTree right = freqTreeQueue.top();
		freqTreeQueue.pop();

		freqTreeQueue.push(new HuffmanFreqTree{
			HUFFMAN_NOT_LEAF,
			left->count + right->count,
			left,
			right
		});
	}

	return freqTreeQueue.top();
}


static void huffmanFreqTreeClean(const pHuffmanFreqTree root) {
	if (root->left) {
		huffmanFreqTreeClean(root->left);
	}
	if (root->right) {
		huffmanFreqTreeClean(root->right);
	}
	delete root;
}

// recursively puts tree data into array
static size_t huffmanFreqTreeRootPutArray(const pHuffmanFreqTree root, unsigned short* binArray, size_t nodeIdx = 0) {
	if (!root) {
		return 0;
	}
	
	binArray[nodeIdx++] = root->value;
	
	if (root->left) {
		nodeIdx = huffmanFreqTreeRootPutArray(root->left, binArray, nodeIdx);
	}
	
	if (root->right) {
		nodeIdx = huffmanFreqTreeRootPutArray(root->right, binArray, nodeIdx);
	}
	
	return nodeIdx;
}

static pHuffmanDict huffmanFreqTreeToDict(const pHuffmanFreqTree root) {
	size_t treeNodeCount = huffmanFreqTreeNodeCount(root);
	pHuffmanDict dict = (pHuffmanDict)new char[sizeof(HuffmanDict) + treeNodeCount - 1]; // compliance with `delete[]`
	dict->size = treeNodeCount;
	huffmanFreqTreeRootPutArray(root, dict->data);
	return dict;
}

static void huffmanDictClean(const pHuffmanDict dict) {
	delete[] (char*)dict; // this should work, or not
}

static std::pair<pHuffmanDict, std::vector<unsigned char>> huffmanEncode(unsigned char* data, size_t dataLen) {
	pHuffmanFreqTree tree = huffmanFreqTreeFromData(data, dataLen);
	pHuffmanDict dict = huffmanFreqTreeToDict(tree);

	std::vector<unsigned char> encData;
	std::bitset<64> bits(0ull);


	
	huffmanFreqTreeClean(tree);

}
