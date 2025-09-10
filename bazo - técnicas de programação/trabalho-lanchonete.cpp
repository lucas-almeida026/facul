#include <iostream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

struct Item {
	string name;
	uint price_in_cents;
};

const vector<Item> lanches = {
	{"Hambúrguer", 1000},
	{"Cheeseburguer", 1200},
	{"X-Bacon", 1500},
};

const vector<Item> bebidas = {
	{"Refrigerante", 500},
	{"Suco", 700},
	{"Água", 300},
};

const vector<Item> sobremesas = {
	{"Sorvete", 800},
	{"Bolo", 600},
	{"Pudim", 500},
};

string itemFormatedPrice(const Item& item) {
	char formated_price[10];
	snprintf(formated_price, sizeof(formated_price), "%.2f", item.price_in_cents / 100.0);
	return formated_price;
}

vector<string> itemsAsOptions(const vector<Item>& items) {
	vector<string> options;
	for (const auto& item : items) {
		options.push_back(item.name + " - R$" + itemFormatedPrice(item));
	}
	return options;
}

uint calcSubtotal(const vector<Item>& items) {
	uint total = 0;
	for (const auto& item : items) {
		total += item.price_in_cents;
	}
	return total;
}

uint calcTotal(const vector<Item>& items) {
	uint subtotal = calcSubtotal(items);
	if (subtotal >= 5000) {
		return subtotal * 0.9; // 10% de desconto para pedidos acima de R$50,00
	}
	return subtotal;
}

string startServiceWithName() {
	cout << "Bem-vindo à lanchonete!" << endl;
	cout << "Por favor, insira seu nome: ";
	string name;
	cin >> name;
	return name;
}

void endService(const string& customerName, const vector<Item>& order) {
	if (order.empty()) {
		cout << "Nenhum item foi pedido. Até logo, " << customerName << "!" << endl;
		return;
	}
	printf("\n");
	cout << "Obrigado pelo seu pedido, " << customerName << "!" << endl;
	cout << "Resumo do pedido:" << endl;
	for (const auto& item : order) {
		string formatedPrice = itemFormatedPrice(item);
		cout << "- " << item.name << ": R$" << formatedPrice << endl;
	}
	uint subtotal = calcSubtotal(order);
	uint total = calcTotal(order);
	cout << "Subtotal: R$" << subtotal / 100.0 << endl;
	if (total < subtotal) {
		cout << "Desconto aplicado! Novo total: R$" << total / 100.0 << endl;
	} else {
		cout << "Total: R$" << total / 100.0 << endl;
	}
	cout << "Volte sempre!" << endl;
}

void printOptions(const vector<string>& options) {
	for (size_t i = 0; i < options.size(); i++) {
		cout << i + 1 << ". " << options[i] << endl;
	}
}


uint getChoiceSync(const vector<string>& options, const string& prompt, const string& header = "") {
	if (!header.empty()) {
		cout << header << endl;
	}
	printf("\n");
	printOptions(options);
	cout << prompt << "(1-" << options.size() << "): ";
	uint choice = 0;
	cin >> choice;
	while (choice == 0 || choice > options.size()) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Entrada inválida. Por favor, insira um número: ";
		} else {
			cout << "Opção inválida. Tente novamente: ";
		}
		cin >> choice;
	}
	return choice;
}

uint getChoiceOrGoBackSync(const vector<string>& options, const string& prompt, const string& header = "") {
	vector<string> optionsWithBack = options;
	optionsWithBack.push_back("Voltar");
	uint originalChoice = getChoiceSync(optionsWithBack, prompt, header);
	if (originalChoice == optionsWithBack.size()) {
		return 0; // representa opção de voltar
	}
	return originalChoice;
}

int main() {
	string customerName = startServiceWithName();
	cout << "Olá, " << customerName << "! Vamos começar seu pedido." << endl;
	vector<Item> order;

	vector<string> menuOptions = {
		"Lanches",
		"Bebidas", 
		"Sobremesas",
		"Finalizar pedido"
	};
	uint choice = getChoiceSync(menuOptions, "Escolha uma categoria, ou finalize o pedido ");

	while (choice != 4) {
		switch (choice) {
			case 1: {
				uint itemChoice = getChoiceOrGoBackSync(itemsAsOptions(lanches), "Escolha um lanche ");
				if (itemChoice == 0) {
					choice = getChoiceSync(menuOptions, "Escolha uma categoria, ou finalize o pedido ");
					break;
				}
				order.push_back(lanches[itemChoice - 1]);
				cout << lanches[itemChoice - 1].name << " adicionado ao pedido." << endl;
				break;
			}
			case 2: {
				uint itemChoice = getChoiceOrGoBackSync(itemsAsOptions(bebidas), "Escolha uma bebida ");
				if (itemChoice == 0) {
					choice = getChoiceSync(menuOptions, "Escolha uma categoria, ou finalize o pedido ");
					break;
				}
				order.push_back(bebidas[itemChoice - 1]);
				cout << bebidas[itemChoice - 1].name << " adicionado ao pedido." << endl;
				break;
			}
			case 3: {
				uint itemChoice = getChoiceOrGoBackSync(itemsAsOptions(sobremesas), "Escolha uma sobremesa ");
				if (itemChoice == 0) {
					choice = getChoiceSync(menuOptions, "Escolha uma categoria, ou finalize o pedido ");
					break;
				}
				order.push_back(sobremesas[itemChoice - 1]);
				cout << sobremesas[itemChoice - 1].name << " adicionado ao pedido." << endl;
				break;
			}
			default:
				cout << "Opção inválida. Tente novamente." << endl;
				break;
		}
	}
	endService(customerName, order);

	return 0;
}