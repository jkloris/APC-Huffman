#include <iostream>
#include <fstream>
#include <map>
#include <optional>
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
		//x = static_cast<unsigned char>(key);
		std::cout << static_cast<int>( key) << ": " << value << '\n';
	}
}

//testing function
int calcBitLength(std::map<unsigned char, std::string> codes, std::map<unsigned char, int> occur) {
	int sum = 0;

	for (const auto& [key, value] : codes)
	{
		sum += static_cast<unsigned int>(value.size()) * occur[key];
	}

	return sum;

}

//int main(int argc, char* argv[])
int main()
{

	std::optional<std::map<unsigned char, int>> optoccur = findOccurancies("input.txt");
	if (!optoccur)
		return EXIT_FAILURE;
	auto occurencies = *std::move(optoccur);


	std::priority_queue<Node> pq;
	for (const auto& [key, value] : occurencies)
	{
		pq.push(Node{ value, key });
	}

	Node x;
	
	// build tree
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

	//printBT("", &tree, false);

	std::map<unsigned char, std::string> codes;
	getCodes( &tree, &codes, "");
	
	printCodes(codes);

	std::cout << "Size = " << calcBitLength(codes, occurencies) << "\n";
	return 0;

}
