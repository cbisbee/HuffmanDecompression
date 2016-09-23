#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <queue>

using namespace std;

string inFileName;
const string universalIdentifier = "BIZCOMPRESS";

struct huffNode
{
	int frequency;
	char character;
	huffNode* left;
	huffNode* right;
};

struct node_comparison
{
	bool operator () (const huffNode* A, const huffNode* B) const
	{
		return A->frequency > B->frequency;
	}
};


void initializeFrequencyList(int freqList[256])
{
	for (int i = 0; i < 256; i++)
	{
		freqList[i] = 0;
	}
}

void generateFrequencyList(int freqList[256], ifstream &fin)
{
	int ch;
	int frequency;
	for (int i = 0; i < 256; i++)
	{
		fin >> ch;
		fin >> frequency;
		freqList[ch] = frequency;
	}

}

void printFrequencyList(int freqList[256])
{
	for (int i = 0; i < 256; i++)
	{
		cout << i << ":" << freqList[i] << endl;
	}
}

void generateInitialPQueue(int frequencyList[256], priority_queue<huffNode*, vector<huffNode*>, node_comparison> &nodeHeap)
{
	for (int i = 0; i < 256; i++)
	{
		if (frequencyList[i] != 0)
		{
			huffNode* tempNode;
			tempNode = new huffNode;
			tempNode->frequency = frequencyList[i];
			tempNode->character = (char)i;
			tempNode->left = NULL;
			tempNode->right = NULL;
			nodeHeap.push(tempNode);
		}
	}
}

void generateHuffmanTree(priority_queue <huffNode*, vector<huffNode*>, node_comparison> &heap)
{
	if (heap.size() <= 1)
		return;
	else
	{
		huffNode *node1, *node2;
		node1 = new huffNode;
		node2 = new huffNode;

		node1 = heap.top();
		heap.pop();
		node2 = heap.top();
		heap.pop();
		int totalFreq = node1->frequency + node2->frequency;

		huffNode* newNode;
		newNode = new huffNode;
		newNode->frequency = totalFreq;
		newNode->character = (char)255;
		newNode->left = node1;
		newNode->right = node2;

		heap.push(newNode);
		totalFreq = 0;
		generateHuffmanTree(heap);
	}
}

void generateEncodings(huffNode *root, vector<bool> & encoding, vector<vector<bool>> & encodingTable)
{
	if (root != NULL) {
		if (root->character != -1)
		{
			encodingTable[root->character] = encoding;
		}
		vector<bool> leftEncoding = encoding;
		leftEncoding.push_back(false);
		vector<bool> rightEncoding = encoding;
		rightEncoding.push_back(true);

		generateEncodings(root->left, leftEncoding, encodingTable);
		generateEncodings(root->right, rightEncoding, encodingTable);
	}
}

void searchEncodings(vector<bool> bitString, vector<vector<bool>> encodingTable,char &asciiChar)
{
	asciiChar = -1;
	for (int i = 0; i < 256; i++)
	{
		if (bitString == encodingTable[i])
		{
			asciiChar = (char)i;
		}
	}
}

void checkEncoding(vector<bool> bitString, vector<vector<bool>> encodingTable, ofstream &fout)
{
	//vector<bool>::const_iterator first = bitString.begin();
	int searchStart = 0;
	char outputCh = -1;
	vector<bool> buffer;
	for (int i = 0; i < bitString.size(); i++)
	{
		//vector<bool> subString(first, first + i);
		buffer.push_back(bitString[i]);
		searchEncodings(buffer, encodingTable, outputCh);
		if (outputCh != -1)
		{
			fout << outputCh;
			buffer.clear();
		}
	}
}

void convertFile(ifstream &fin, ofstream &fout, vector<bool> &bitString, vector<vector<bool>> encodingTable)
{
	char byte;
	char outputCh;
	//vector<bool> bitString;
	//vector<bool> buffer;

	while (fin.read(&byte, 1))
	{
		for (int i = 0; i < CHAR_BIT; i++)
		{
			bitString.push_back(((byte >> i) & 1) != 0);
		}
		//byte = -1;
		//checkEncoding(bitString, outputCh, encodingTable);
		/*if (outputCh != -1);
		{
			fout << outputCh;
		}*/
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "No file specified, rerun the application and provide a .mcp file";
	}
	else
	{
		inFileName = argv[1];
		ifstream fin(inFileName, ios::binary);
		ofstream fout("output.txt"); //need to make this the same name as the input file!!!!!!!!!
		string extension = inFileName.substr(inFileName.length() - 4, 4);
		priority_queue<huffNode*, vector<huffNode*>, node_comparison> nodeHeap;
		vector<bool> startEncoding;
		vector<bool> bitBuffer;
		vector<vector<bool>> encodingTable(256, vector<bool>(0));


		if (fin.fail() || extension != ".mcp")
		{
			cout << "File supplied was not an .mcp file, rerun program and provide a .mcp file";
		}
		else
		{
			string compressionIdentifier;
			fin >> compressionIdentifier;
			if (compressionIdentifier != universalIdentifier)
			{
				cout << "Compressed file does not have correct file identifier. Please ensure that the file was compressed by it's sister program";
				return 0;
			}

			string originalFileName;
			fin >> originalFileName;
			cout << "The original file name was: " << originalFileName << endl;

			int frequencyList[256];
			

			initializeFrequencyList(frequencyList);
			generateFrequencyList(frequencyList, fin);

			generateInitialPQueue(frequencyList, nodeHeap);
			generateHuffmanTree(nodeHeap);

			huffNode *root;
			root = nodeHeap.top();
			nodeHeap.pop();

			//create the encoding for each leaf node (character)
			generateEncodings(root, startEncoding, encodingTable);
			
			convertFile(fin, fout, bitBuffer, encodingTable);
			checkEncoding(bitBuffer, encodingTable, fout);


		}
	}
	cin.get();
	cin.get();

	return 0;
}