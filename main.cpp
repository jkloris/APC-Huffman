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

		mod = argv[1];
		if (!((mod == "--print" && argc == 3) || (mod == "--compress" && argc == 4)) && mod != "--decompress")
			return false;

		inputPath = argv[2];
		if(argc == 4)
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




std::optional<std::vector<int>> findOccurancies(std::string const &inpath) {

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

	inf.seekg(0, std::ios::beg); //TODO skontrolovat

	std::vector<int> occuranciesArr(256);


	char buffer[32768];
	std::streamsize bufferSize = std::size(buffer);
	inf.read(buffer, bufferSize);
	size_t buffi = 0, gc = inf.gcount();

	while (gc > 0) {
		while (buffi < gc) {
			occuranciesArr[static_cast<unsigned char>(buffer[buffi])]++;
			buffi++;
		}
		inf.read(buffer, bufferSize);
		gc = inf.gcount();
		buffi = 0;
	}

	inf.close();
	return occuranciesArr;
}

void getCodes(const Node *tree, std::map<unsigned char, std::string> *codes,std::string cd) {
	if (tree->left == NULL || tree->right == NULL) {
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
		std::cout << static_cast<int>(key) << ": " << value << '\n';
		//std::cout << static_cast<int>( key) << " " << key << ": " << value << '\n';
	}
}

uint64_t calcBitLength(std::vector<std::string> codes, std::vector<int> occur) {
	uint64_t sum = 0;
	size_t len = codes.size();

	for (uint16_t i = 0; i < len; i++  )
	{
		sum += codes[i].size() * occur[i];
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


bool compressFile( std::vector< std::string> codesArr, std::vector<int> occur, std::string const &inpath, std::string const &outpath) {
	std::ofstream outfile(outpath, std::ios::binary);
	

	//std::cout << "compress\n";

	uint64_t size = calcBitLength(codesArr, occur);
	outfile.write(reinterpret_cast<char*>(&size), sizeof(size));

	if (!outfile.is_open())
		return false;

	char zero = 0;
	char nonzeroSize = 1;
	std::vector< char> bytes = {  };

	std::string code = "";
	code.reserve(256);
	
	// writing -> code length | code 
	for (unsigned char c = 0; ; c++) {
		code = codesArr[c];
			
		if (code.length() == 0) {
			outfile.write(reinterpret_cast<char*>(&zero), sizeof(c));
		}
		else {
			nonzeroSize = static_cast<char> (code.length());
			outfile.write(&nonzeroSize, sizeof(c));

			bytes = codeToBin(code);
			for (char b : bytes) {
				outfile.write(&b, sizeof(b));
			}
		}
		// manual break, because char cant be more than 255
		if (c == 255) break;
	}

	std::ifstream infile(inpath, std::ios::binary);
	if (!infile.is_open()) {
		std::cout << "Unable to open file\n"; // TODO del
		return  false;
	}


	

	char buffer[32768];
	std::streamsize bufferSize = std::size(buffer);
	infile.read(buffer, bufferSize);
	size_t buffi = 0, gc = infile.gcount(); 
	
	uint16_t bi = 0;
	uint8_t  byte = 0;

	
	// writing compressed code
	while (gc > 0) {
		while (buffi < gc) {

			code = codesArr[static_cast<unsigned char>(buffer[buffi])];
			//code = codes[buffer[buffi]];  //slowest part
			
			for (auto c : code) {
				byte |= (c - '0') << (7 - bi);
				bi++;

				if (bi >= 8) {
					bi = 0;
					outfile.write(reinterpret_cast<char*>(&byte), sizeof(char));
					byte = 0;
				}
			}

			buffi++;

		}
		infile.read(buffer, bufferSize);
		gc = infile.gcount();
		buffi = 0;
	}
	if(bi > 0)
		outfile.write(reinterpret_cast<char*>(&byte), sizeof(char));


	infile.close();
	outfile.close();

	return true;
}

int main(int argc, char* argv[])
{

	ArgumentHandler arguments;
	if (!arguments.setArguments(argc, argv))
		return EXIT_FAILURE;

 	std::optional<std::vector<int>> optoccur = findOccurancies(arguments.inputPath);
	if (!optoccur)
		return EXIT_FAILURE;
	auto occurencies = *std::move(optoccur);


	// build tree--------
	std::priority_queue<Node> pq;
	size_t len = occurencies.size();

	for (uint16_t i = 0; i < len; i++)
	{
		if (occurencies[i] == 0) continue;
		pq.push(Node{ occurencies[i], static_cast<unsigned char>(i) });
	}


	Node x;
	std::vector<Node*> allocatedNodes;


	while (pq.size() > 1) {
		Node *l = new Node();
		*l = pq.top();
		pq.pop();

		Node *r = new Node();
		*r = pq.top();
		pq.pop();
		
		allocatedNodes.push_back(r);
		allocatedNodes.push_back(l);

		x.value = l->value + r->value;
		x.left = l;
		x.right = r;
		x.c = '\0';

		pq.push(x);		 
	}

	Node tree = pq.top();
	//-------------- TODO free
	//printBT("", &tree, false);

	std::map<unsigned char, std::string> codes;
	getCodes( &tree, &codes, "");

	//free tree
	for (auto it : allocatedNodes) {
		delete it;
	}


	std::vector<std::string> codesArr(256);
	for (const auto& [key, value] : codes) {

		codesArr[key] = value;
	}



	if (arguments.mod == "--print")
		printCodes(codes);
	else if (arguments.mod == "--compress")
		if (!compressFile(codesArr, occurencies, arguments.inputPath, arguments.outputPath))
			return EXIT_FAILURE;

	return 0;
}
