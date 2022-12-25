#include <iostream>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <queue>
#include <bitset>

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
	

	// TODO mozno osetrit subor uz tu (vysputny sa zatial kontroluje len ak je --compress)
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




std::optional<std::map<unsigned char, int>> findOccurancies(std::string const &inpath) {

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
	


	char buffer[4096];
	std::streamsize bufferSize = std::size(buffer);
	inf.read(buffer, bufferSize);
	size_t buffi = 0, gc = inf.gcount();

	while (gc > 0) {
		while (buffi < gc) {
			occurancies[buffer[buffi]]++;
			buffi++;
		}
		inf.read(buffer, bufferSize);
		gc = inf.gcount();
		buffi = 0;
	}

	/*char c;
	while (inf.read(&c, 1)) {

		occurancies[c]++;
	}*/


	inf.close();
	return occurancies;

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

bool compressFile(std::map<unsigned char, std::string>  codes, std::map<unsigned char, int> occur, std::string const &inpath, std::string const &outpath) {
	std::ofstream outfile(outpath, std::ios::binary);


	uint64_t size = calcBitLength(codes, occur);
	outfile.write(reinterpret_cast<char*>(&size), sizeof(size));

	if (!outfile.is_open())
		return false;

	char zero = 0;
	char nonzeroSize = 1;
	std::vector< char> bytes = {  };
	
	std::cout << "codes\n";
	
	for (unsigned char c = 0; ; c++) {
		if (codes[c].length() == 0) {
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
		std::cout << "Unable to open file\n"; // TODO del
		return  false;
	}

	std::cout << "compress\n";

	//test 
	//char buffer[65536];
	//std::streamsize bufferSize = std::size(buffer);
	////outfile.write(reinterpret_cast<char*>(&buffer), bufferSize);
	//infile.read(buffer, bufferSize);
	//size_t buffi = 0, gc = infile.gcount();

	//char c = 0;
	//size_t i = INT_MAX, b = 0;
	//nonzeroSize = 0;
	////std::string tmp = codes[c];
	//while (gc > 0) {
	//	while (buffi < gc) {
	//		
	//		
	//		if (i >= codes[c].length()) {
	//			c = buffer[buffi];
	//			i = 0;
	//			//tmp = codes[c];
	//			buffi++;
	//		}

	//		nonzeroSize |= (codes[c][i] - '0') << (7 - b);
	//		b++;
	//		i++;
	//		if (b == 8) {
	//			outfile.write(&nonzeroSize, sizeof(char));
	//			nonzeroSize = 0;
	//			b = 0;
	//		}
	//	
	//		
	//	}
	//	infile.read(buffer, bufferSize);
	//	gc = infile.gcount();
	//	buffi = 0;
	//}
	//outfile.write(&nonzeroSize, sizeof(char));
	//----


	//test  2

	char buffer[8192];
	std::streamsize bufferSize = std::size(buffer);
	infile.read(buffer, bufferSize);
	size_t buffi = 0, gc = infile.gcount(), bitbufSize = 8, writtenBitsLeft = size;
	
	//char c = 0;
	//uint64_t  b = 0;
	std::string tmp = "";
	tmp.reserve(bitbufSize + 256);

	while (gc > 0) {
		while (buffi < gc) {


			tmp += codes[buffer[buffi]];
			
			//if (tmp.size() >= 8 && writtenBitsLeft < bitbufSize) {
			//	std::bitset<8> bits(tmp.substr(0, 8));
			//	outfile.write(reinterpret_cast<char*>(&bits), 1);
			//	tmp.erase(0, 8);
			//	writtenBitsLeft -= 8;
			//}
			if (tmp.size() >= bitbufSize) {
				std::bitset<8> bits(tmp.substr(0, bitbufSize));
		

				outfile.write(reinterpret_cast<char*>(&bits), bitbufSize / 8);
				tmp.erase(0, bitbufSize);
				writtenBitsLeft -= bitbufSize;
			}
			buffi++;

		}
		infile.read(buffer, bufferSize);
		gc = infile.gcount();
		buffi = 0;
	}


	while (tmp.size() > 0) {
		std::bitset<8> bits(tmp.substr(0, 8));
		outfile.write(reinterpret_cast<char*>(&bits), 1);
		tmp.erase(0, 8);
	}
	//outfile.write(reinterpret_cast<char*>(&b), sizeof(b));
	//


	/*size_t i = INT_MAX, b = 0;
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
	outfile.write(&nonzeroSize, sizeof(char));*/

	infile.close();
	outfile.close();

	return true;
}

int main(int argc, char* argv[])
{

	ArgumentHandler arguments;
	if (!arguments.setArguments(argc, argv))
		return EXIT_FAILURE;

	////test
	/*
	std::ofstream outfile(arguments.outputPath, std::ios::out |std::ios::binary);
	uint64_t buffer;
	//std::streamsize bufferSize = std::size(buffer);
	buffer = std::stoull("1111111101110001011100010111000101110000000000000000000000000001", nullptr, 2);
	//buffer[0] |= 1 << (sizeof(buffer[0])*8 - 1);
	//buffer[0] |= 7 << (sizeof(buffer[0]) - 10);
	outfile.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
	outfile.close();

	size_t size = 16;
	std::string s = "111101100", tms;
	//std::bitset<128> bits = {0};
	for (size_t i = 0; i < 14000000; i++) {
		s += "1100";
		if (s.size() >= size) {
			std::bitset<8192> bits(s.substr(0, size));
			outfile.write(reinterpret_cast<char*>(&bits), size /8);
			s.erase(0, size);
			outfile.close();
		}
	}
	std::bitset<8> bits(s.substr(0, 8));
	outfile.write(reinterpret_cast<char*>(&bits), 1);
	//std::cout<< sizeof(buffer[0]) << " " << (15 << 4) << " " << (15 << 5) << " " << (15 << 6);
	//


	outfile.close();
	*/
	//-------


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
	//-------------- TODO free
	//printBT("", &tree, false);

	std::map<unsigned char, std::string> codes;
	getCodes( &tree, &codes, "");


	if (arguments.mod == "--print")
		printCodes(codes);
	else if (arguments.mod == "--compress")
		if (!compressFile(codes, occurencies, arguments.inputPath, arguments.outputPath))
			return EXIT_FAILURE;

	//std::cout << "Size = " << calcBitLength(codes, occurencies) << "\n";
	return 0;

}
