
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

//int main(int argc, char* argv[])
int main()
{

	
	std::priority_queue<Node> pq;
	for (const auto& [key, value] : findOccurancies("aaabcdeeeeeeeeddddddd"))
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
	getCodes( &tree, &codes, " ");

	return 0;

}
