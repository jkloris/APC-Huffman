#include <iostream>
#include <map>
#include <queue>


struct Node
{
	int value;
	char c;
	struct Node* left;
	struct Node* right;

};

bool operator<(const Node& lhs, const Node& rhs)
{
	if (lhs.value == rhs.value)
		return lhs.c > rhs.c;
	return lhs.value > rhs.value;

}



//_____________________________
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

std::map<char, int> findOccurancies(std::string str) {
	std::map<char, int> occurancies = {};
	
	for (size_t i = 0; i < str.size(); i++) {
		occurancies[str[i]]++;
		
	}

	return occurancies;

}

void getCodes(Node *tree, std::map<char, std::string> *codes,std::string cd) {
	if (tree->c != NULL) {
		codes->insert(std::pair<char, std::string>(tree->c, cd));
		return;
	}

	if (tree->left != NULL) {
		getCodes(tree->left, codes, cd + '0');
	}
	
	if (tree->right != NULL) {
		getCodes(tree->right, codes, cd + '1');
	}

}

void printCodes(std::map<char, std::string> codes) {
	for (const auto& [key, value] : codes)
	{
		std::cout << static_cast<int>( key) << ": " << value << '\n';
	}
}

//testing function
int calcBitLength(std::map<char, std::string> codes, std::map<char, int> occur) {
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

	
	std::priority_queue<Node> pq;
	std::map<char, int> occurencies = findOccurancies("man coding uses a variable length code for each of the elements within the data. This normally involves analyzing the data to determine the probability of its elements. The most probable elements are coded with a few bits and the least probable coded with a greater number of bits. This could be done on a character-by-character basis, in a text file, or could be achieved on a byte-by-byte basis for ot");
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

	printBT("", &tree, false);

	std::map<char, std::string> codes;
	getCodes( &tree, &codes, "");
	
	printCodes(codes);

	std::cout << "Size = " << calcBitLength(codes, occurencies) << "\n";
	return 0;

}
