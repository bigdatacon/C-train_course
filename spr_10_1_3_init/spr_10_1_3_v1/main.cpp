// напишите решение с нуля
// код сохраните в свой git-репозиторий

int main() {
	deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
	TransportCatalogue tc;
	ir = InputReader(cin);
	UpdateQuery q;
	int query_count = ir.getNumUpdateQueries();


	for (int i = 0; i < query_count; ++i) {
		q = ir.getUpdateQuery();
		switch (q.type) {
		case QueryType::Bus:
			tc.AddBus(q);
		case QueryType::Stop:
			tc.AddStop(q);
		}

	}

	return 0;
};
