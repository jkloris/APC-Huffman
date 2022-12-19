#include <iostream>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <queue>


struct Node
{
	int value;
	unsigned char c;
	struct Node* left;
	struct Node* right;

};

bool operator<(const Node& lhs, const Node& rhs)
{
	if (lhs.value == rhs.value)
		return lhs.c > rhs.c;
	return lhs.value > rhs.value;

}

class ArgumentHandler {
public:
	std::string mod = {  };
	std::string inputPath;
	std::string outputPath;
	
	bool setArguments(int argc, char* argv[]) {
		if (argc != 4)
			return false;

		mod = argv[1];
		if (mod != "--print" && mod != "--compress" && mod != "--decompress")
			return false;

		inputPath = argv[2];
		outputPath = argv[3];
		return true;
	}
};



//_____________________________tmp test print
void printBT(const std::string& prefix, const Node* node, bool isLeft)
{
	if (node != nullptr)
	{
		std::cout << prefix;

		std::cout << (isLeft ? "|--" : "|--");

		// print the value of the node
		std::cout << node->value << std::endl;

		// enter the next tree level - left and right branch
		printBT(prefix + (isLeft ? "|   " : "    "), node->left, true);
		printBT(prefix + (isLeft ? "|   " : "    "), node->right, false);
	}
}



//________________________




std::optional<std::map<unsigned char, int>> findOccurancies(std::string inpath) {

	std::ifstream inf(inpath, std::ios::binary);
	if (!inf.is_open()) {
		std::cout << "Unable to open file\n";
		return  {};
	}

	std::streampos currPos = inf.tellg();
	inf.seekg(0, std::ios::end);
	if (inf.tellg() == currPos) {
		std::cout << "Empty file\n";
		return {};
	}
	inf.seekg(0, 0);

	std::map<unsigned char, int> occurancies = {};
	char c;
	while (inf.read(&c, 1)) {

		occurancies[c]++;
	}


	inf.close();
	return occurancies;

}

void getCodes(Node *tree, std::map<unsigned char, std::string> *codes,std::string cd) {
	if (tree->c != NULL) {
		codes->insert(std::pair<unsigned char, std::string>(tree->c, cd));
		return;
	}

	if (tree->left != NULL) {
		getCodes(tree->left, codes, cd + '0');
	}
	
	if (tree->right != NULL) {
		getCodes(tree->right, codes, cd + '1');
	}

}


void printCodes(std::map<unsigned char, std::string> codes) {

	for (const auto& [key, value] : codes)
	{
		std::cout << static_cast<int>( key) << " " << key << ": " << value << '\n';
	}
}

uint64_t calcBitLength(std::map<unsigned char, std::string> codes, std::map<unsigned char, int> occur) {
	uint64_t sum = 0;

	for (const auto& [key, value] : codes)
	{
		sum += (value.size()) * occur[key];
	}

	return sum;

}

std::vector< char> codeToBin(std::string strcode) {

	std::vector< char> bytes;
	char byte = 0;


	
	for (size_t i = 0; i < strcode.length(); ) {
		byte = 0;
		for (size_t b = 0; b < 8 && i < strcode.length(); b++, i++) {
			byte |= (strcode[i]-'0') << (7 - b);
		}
		bytes.push_back(byte);
	}

	return bytes;
}

void compressFile(std::map<unsigned char, std::string> codes, std::map<unsigned char, int> occur, std::string inpath, std::string outpath) {
	std::ofstream outfile(outpath, std::ios::binary);
	

	uint64_t size = calcBitLength(codes, occur);
	outfile.write(reinterpret_cast<char*>(&size), sizeof(size));
	
	char zero = 0;
	char nonzeroSize = 1;
	std::vector< char> bytes = {  };

	for (unsigned char c = 0; c < 256 ; c++) {
		if( codes[c].length() == 0) {
			outfile.write(reinterpret_cast<char*>(&zero), sizeof(c));
		}
		else {
			nonzeroSize = static_cast<char> (codes[c].length());
			outfile.write(&nonzeroSize, sizeof(c));
			
			bytes = codeToBin(codes[c]);
			for (char b : bytes) {
				outfile.write(&b, sizeof(b));
			}
		}
		// manual break, because char cant be more than 255
		if (c == 255) break;
	}
	
	std::ifstream infile(inpath, std::ios::binary);
	if (!infile.is_open()) {
		std::cout << "Unable to open file\n";
		//return  {}; TODO
	}


	size_t i = INT_MAX, b = 0;
	nonzeroSize = 0;
	char c = 0;
	std::string tmp = codes[c];
	while (1) {
		if (i >= codes[c].length()) {
			if (!infile.read(&c, 1)) break;
			i = 0;
			tmp = codes[c];
		}

		nonzeroSize |= (codes[c][i] - '0') << (7 - b);
		b++;
		i++;
		if (b == 8 ) {
			outfile.write(&nonzeroSize, sizeof(char));
			nonzeroSize = 0;
			b = 0;
		}
		
	}
	outfile.write(&nonzeroSize, sizeof(char));

	infile.close();
	outfile.close();
}

int main(int argc, char* argv[])
{
	// TODO args handling
	ArgumentHandler arguments;
	if (!arguments.setArguments(argc, argv))
		return EXIT_FAILURE;


	std::optional<std::map<unsigned char, int>> optoccur = findOccurancies(arguments.inputPath);
	if (!optoccur)
		return EXIT_FAILURE;
	auto occurencies = *std::move(optoccur);


	// build tree--------
	std::priority_queue<Node> pq;
	for (const auto& [key, value] : occurencies)
	{
		pq.push(Node{ value, key });
	}

	Node x;
	
	while (pq.size() > 1) {
		Node *l = new Node();
		*l = pq.top();
		pq.pop();

		Node *r = new Node();
		*r = pq.top();
		pq.pop();
		
		x.value = l->value + r->value;
		x.left = l;
		x.right = r;
		x.c = NULL;

		pq.push(x);		 
	}

	Node tree = pq.top();
	//--------------
	//printBT("", &tree, false);

	std::map<unsigned char, std::string> codes;
	getCodes( &tree, &codes, "");


	if(arguments.mod == "--print")
		printCodes(codes);
	else if(arguments.mod == "--compress")
		compressFile(codes, occurencies, arguments.inputPath, arguments.outputPath);

	//std::cout << "Size = " << calcBitLength(codes, occurencies) << "\n";
	return 0;

}
