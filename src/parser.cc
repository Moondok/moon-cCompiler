
#include <parser.h>
//semantic global
//---------------------------------------//
std::stack<word> wordS;//temp store word
std::stack<sym_tbl> tableS;//symbol table

void Parser::read_grammer_Yacc(const std::string path)
{
	std::fstream f(path, std::ios::in);
	if (!f.is_open())
	{
		std::cerr << "FILE WARNING::OBJECT FILE CAN NOT BE OPENED!\n";
		return;
	}
	std::fstream token_file(".//yacc_tokens.txt", std::ios::out);
	std::fstream non_file(".//yacc_non.txt", std::ios::out);
	std::fstream generators_file(".//generators.txt", std::ios::out);


	std::string tmp;
	bool read_token = true;
	unsigned int pos=0;
	unsigned int end_pos = 0;


	num_nontermi = 1; // for we need add a start

	//the same trick with the moon-parser1.0 ,we traverse the grammer for 2 passes
	// the 1st pass
	while (f.eof() == false)
	{
		getline(f,tmp);
		if (tmp.empty())
			continue;

		
		// read in all the tokens (terminator)
		if (read_token)
		{
			tmp += ' ';
			if (tmp.at(0) == '%')
				if (tmp.at(1) == '%')
					read_token = false;
				else if(tmp.at(1)!='s')
				{
					pos = tmp.find_first_of(' ');
					pos++;
					while (pos != tmp.size())
					{
						end_pos = tmp.find_first_of(' ', pos);
						symbol new_terminator(tmp.substr(pos, end_pos - pos), TERMINATOR);
						this->num_termi++;
						this->terminators.insert(new_terminator);
						this->symbols.insert(new_terminator);
						pos = end_pos + 1;
					}
				}
		}

		//read in geenrators
		else
			if (tmp.at(0) != '\t') // a new non_terminator
			{
				symbol new_nontermi(tmp, NON_TERMINATOR, num_nontermi++);
				this->non_terminators.insert(new_nontermi);
				symbols.insert(new_nontermi);
				
			}
	}
	f.close();

	symbol new_start("start", NON_TERMINATOR, 0);
	non_terminators.insert(new_start);
	this->symbols.insert(new_start);
	generator augmented_generator;
	augmented_generator.left = new_start;

	auto ite = non_terminators.find(symbol("translation_unit"));
	augmented_generator.right_list.emplace_back(*ite);
	std::vector<generator> first_list;
	first_list.emplace_back(augmented_generator);
	generators_list.emplace_back(first_list);
	pure_generator_list.emplace_back(augmented_generator);


	//2nd pass
	pos = 0;
	end_pos = 0;
	unsigned int order = 1; // the label of generator //the start order is 1 for we add a new generator before it
	f.open(path, std::ios::in);
	while (f.eof() == false)
	{
		getline(f, tmp);
		if (tmp.empty())
			continue;
		if (tmp.at(0) == '%')
			continue;

		symbol new_nontermi(tmp);

		std::set<symbol>::iterator ite;
		if ((ite=this->non_terminators.find(new_nontermi)) == non_terminators.end())
			std::cerr << "WTF\n";
		new_nontermi = *ite;

		
		//get all generators from one left symbol
		getline(f, tmp);
		tmp.erase(0, 1);
		while (tmp.at(0) != ';')
		{
			pos = end_pos = 0;
			tmp.erase(0, 2);
			generator new_generator;
			std::vector<symbol> right_list;
			new_generator.left = new_nontermi;
			
			tmp += ' ';
			while (pos != tmp.size())
			{
				end_pos = tmp.find_first_of(' ',pos);
				std::string new_symbol_name = tmp.substr(pos, end_pos - pos);
				
				if (new_symbol_name.at(0) == '\'') //analogous to '('
				{
					new_symbol_name = new_symbol_name.substr(1,new_symbol_name.size()-2);
					symbol new_symbol(new_symbol_name, TERMINATOR);
					this->terminators.insert(new_symbol);
					this->symbols.insert(new_symbol);
					right_list.emplace_back(new_symbol);
				}
				else
				{
					symbol new_symbol(new_symbol_name, NON_TERMINATOR);
					ite = this->non_terminators.find(new_symbol);
					if (ite == this->non_terminators.end())
					{
						ite = this->terminators.find(new_symbol);
						//if (ite == this->terminators.end())
							//std::cerr << "?\n";
						new_symbol.type = TERMINATOR;
					}
					else
						new_symbol.generators_index = ite->generators_index;
					right_list.emplace_back(new_symbol);
				}
				pos = end_pos + 1;
			}
			new_generator.order = order++;
			new_generator.right_list = right_list;
			if (this->generators_list.size() <= new_nontermi.generators_index)
			{
				std::vector<generator> new_generator_list;
				new_generator_list.emplace_back(new_generator);
				this->generators_list.emplace_back(new_generator_list);
			}
			else
				this->generators_list.at(new_nontermi.generators_index).emplace_back(new_generator);

			pure_generator_list.emplace_back(new_generator);
			//here we get an generator
			getline(f, tmp);
			tmp.erase(0, 1);

		}
	}

	


	std::cout << num_termi << " " << num_nontermi << "\n";
	for (auto ite = this->non_terminators.begin(); ite != non_terminators.end(); ite++)
	{
		non_file << ite->name << "\n";
	}
	for (auto ite = this->terminators.begin(); ite != terminators.end(); ite++)
	{
		token_file << ite->name << "\n";
	}

	for (const auto i : generators_list)
	{
		for (const auto j : i)
		{
			generators_file <<j.order<<"   :"<<j.left.name << " ";
			generators_file  << " --> ";
			for (const auto k : j.right_list)
				generators_file << k.name << " ";
			generators_file << "\n";
		}
		generators_file << "\n";
	}

	generators_file.close();
	token_file.close();
	non_file.close();
	f.close();

}


void Parser::get_symbol_first(const symbol& a)
{
	//if (a.name == "operator")
		//std::cout << "g";

	if (symbol2first.find(a.name) != symbol2first.end())
		return;

	//start establishing
	std::set<symbol> first_set;

	//if a is an terminator
	if (a.type == TERMINATOR)
	{
		first_set.insert(a);
		symbol2first.insert(std::make_pair(a.name, first_set));
		return;
	}



	int index = a.generators_index;
	// i is a generator

	bool have_epsilon = true; 
	for (auto const& i : generators_list.at(index))
	{
		have_epsilon = true;
		for (auto sym : i.right_list)  
		{
			if (sym.type == TERMINATOR)
			{
				have_epsilon = false;
				first_set.insert(sym);
				break;
			}
			//non-terminator

			/** when we transfer our grammer to yacc c ,we add this we avoid the impact of left recursion*/
			if (sym.name == a.name)
			{
				have_epsilon = false;
				break;
			}
			//**end**/

			if (symbol2first.find(sym.name) == symbol2first.end())
				get_symbol_first(sym);

			auto const &tmp = symbol2first.find(sym.name);  //tmp is an iterator of map
			if (tmp->second.find(symbol("$"))==tmp->second.end()) // the episilon is not in the first set of sym
			{
				have_epsilon = false;
				std::set_union(first_set.begin(),first_set.end(), tmp->second.begin(),tmp->second.end(),std::inserter(first_set,first_set.begin()));
				break;  
			}
			else
			{
				std::set<symbol> temp = tmp->second;
				temp.erase(symbol("$", TERMINATOR, -1));  //ERASE THE EPISILON when merging the first set
				std::set_union(first_set.begin(), first_set.end(), temp.begin(), temp.end(), std::inserter(first_set, first_set.begin()));
			}

		}
		if (have_epsilon == true)
		{
			first_set.insert(symbol("$", TERMINATOR, -1));
		}
	}
	symbol2first.insert(std::make_pair(a.name, first_set));
	return;
}

void Parser::get_all_symbol_first()
{
	for (auto const& i : non_terminators)
		get_symbol_first(i);
	for (auto const& i : terminators)
		get_symbol_first(i);


	std::fstream of;
	of.open("./first_set.txt", std::ios::out);
	for (auto const& i : symbol2first)
	{
		of << "FIRST['" << i.first << "'] = { ";
		for (std::set<symbol>::iterator ite = i.second.begin(); ite != i.second.end(); ite++)
		{
			of <<"'"<<ite->name<<"'" << " ";
		}
		of << "}\n";
	}

	of.close();
}


void Parser::construct_closure(_item_group_& group)
{
	std::set<_item_> is_visited;
	std::set<_item_> is_unvisited(group.items);
	
	std::vector<std::string> re;
	std::vector<std::string> tmp;

	while (is_unvisited.empty() == false)
	{
		tmp.clear();
		re.clear();

		std::set<_item_>::iterator ite = is_unvisited.begin();
		is_visited.insert(*ite);

		if (ite->index < ite->base.right_list.size() && ite->base.right_list.at(ite->index).type == NON_TERMINATOR)
		{
			for (unsigned i = ite->index + 1; i < ite->base.right_list.size(); i++)
				tmp.emplace_back(ite->base.right_list.at(i).name);
			tmp.emplace_back(ite->prospect_symbol);
			get_sequence_first(tmp, re);

			symbol B_symbol = ite->base.right_list.at(ite->index); // in accordance with the mark in textbook
			int generators_index = non_terminators.find(B_symbol)->generators_index;

			for (const auto& i : generators_list.at(generators_index))
			{
				generator new_one(i);
				_item_ new_item(new_one, 0);

				for (const auto& pros_symbol : re)
				{
					new_item.prospect_symbol = pros_symbol;
					if (is_visited.find(new_item) == is_visited.end())
						is_unvisited.insert(new_item);
				}
			}
		}
		is_unvisited.erase(*ite);
	}
	std::set_union(group.items.begin(), group.items.end(), is_visited.begin(), is_visited.end(), std::inserter(group.items, group.items.begin()));

}

void Parser::get_sequence_first(const std::vector<std::string>& seq, std::vector<std::string>& re)
{
	for (auto const& i : seq)
	{
		std::set<symbol> tmp= this->symbol2first.find(i)->second;

		//no epsilon
		if (tmp.find(symbol("$", TERMINATOR, -1)) == tmp.end())
		{
			for (const auto& j : tmp)
				re.emplace_back(j.name);
			break;
		}
		for (const auto& j : tmp)
		{
			if(j.name!="$")
				re.emplace_back(j.name);  //no break here
		}

		//if(this->symbol2first.find(i)->second.find)
	}
}


void Parser::get_state_group_list()
{
	std::fstream of;
	of.open("_state_group_.txt", std::ios::out);

	std::set<symbol> tmp;
	tmp.insert(symbol("#", TERMINATOR, -1));
	symbol2first.insert(std::make_pair("#", tmp));

	_item_group_ first_group(0);

	symbol first_symbol("start", NON_TERMINATOR);
	int index = non_terminators.find(first_symbol)->generators_index;
	_item_ first_item(generators_list.at(index).at(0), 0);
	first_item.prospect_symbol = "#";

	first_group.items.insert(first_item);

	construct_closure(first_group);
	_item_groups_.insert(first_group);

	std::map<std::string, int> first_map;
	go_map.emplace_back(first_map);

	std::queue<_item_group_>q;
	q.push(first_group);

	unsigned int id_track = 0;

	while (q.empty() == false)
	{
		_item_group_ tmp1 = q.front();
		of << tmp1.id << "\n";
		for (auto i : tmp1.items)
		{
			of << "(" << i.base.order << " , '" << i.base.left.name << " , " << "(";
			for (auto j : i.base.right_list)
				of << "'" << j.name << "',";
			of << ") " << i.index << " (";
			of << "'" << i.prospect_symbol << "'";
			of << ')' << ')' << ",  ";

			of << "\n   ";
		}
		of << "\n";

		q.pop();



		for (auto i : symbols)
		{
			_item_group_ new_one;
			//unsigned int existed_id = -1;

			//if (i.name == "translation_unit")
			//	std::cout << "here2\n";
			state_group_go(tmp1, new_one, i.name);

			if (new_one.items.size() != 0 && _item_groups_.find(new_one) == _item_groups_.end())//a new set
			{

				new_one.id = ++id_track;

				/** here we link different item groups and restore them into the"go_map"**/
				std::map<std::string, int> new_map;
				go_map.emplace_back(new_map);
				go_map.at(tmp1.id).insert(std::make_pair(i.name, new_one.id)); // nobody will know what it means


				std::cout << "c status number: " << id_track << "\n";
				_item_groups_.insert(new_one);
				q.push(new_one);
			}
			else
			{
				if (new_one.items.size() != 0)
				{
					int dst_id = _item_groups_.find(new_one)->id;
					go_map.at(tmp1.id).insert(std::make_pair(i.name, dst_id)); // nobody will know what it means *2
				}
			}
		}
	}

	of.close();
	std::cout << "total status number: " << id_track;
}

void Parser::state_group_go(const _item_group_& scr, _item_group_& dst, std::string input)
{
	for (const auto& i : scr.items)
	{
		if (i.index < i.base.right_list.size() && input == i.base.right_list.at(i.index).name) //at least one symbol behind the point 
		{
			_item_ tmp(i.base, i.index + 1);  // the point's position move 

			//for (unsigned int cnt = i.index + 2; cnt < i.base.right_list.size(); cnt++)
			//	temp1.emplace_back(i.base.right_list.at(cnt).name);
			tmp.prospect_symbol = i.prospect_symbol;

			dst.items.insert(tmp);
		}
	}

	//here we derive items from dst's items itself
	construct_closure(dst);
}

void Parser::get_LR1_table()
{
	for (const auto& item_group_unit : _item_groups_)
	{
		movement mov;
		LR1_table.emplace_back(mov);
	}


	
	for (const auto& item_group_unit : _item_groups_)
	{
		std::set<for_verify_LR1> s;

		int index = item_group_unit.id;
		for (const auto& item_unit : item_group_unit.items)
		{
			if (item_unit.index < item_unit.base.right_list.size())
			{

				symbol a_symbol = item_unit.base.right_list.at(item_unit.index);

				if (a_symbol.type == TERMINATOR)
				{
					auto ite = go_map.at(index).find(a_symbol.name);
					if (ite != go_map.at(index).end()) //we get it
					{
						int dst = ite->second * -1;

						auto  it = s.find(for_verify_LR1(index, a_symbol.name,dst));
						if (it == s.end())
						{
							s.insert(for_verify_LR1(index, a_symbol.name, dst));

							action_struct* new_action = new action_struct(a_symbol.name, dst);
							//link this new unit to the index-th position of LR1 map

							//insert into the head!
							action_struct* tmp = LR1_table.at(index).action_ptr->next;
							new_action->next = tmp;
							LR1_table.at(index).action_ptr->next = new_action;
						}
						else
						{
							if (it->dst != dst)
							{
								std::cerr << "\nTHIS IS NOT LR1 GRAMMER!" << index << " " << a_symbol.name << "\n";
								std::cerr << it->dst << " " << dst << " " << item_unit.base.left.name << " "<<item_unit.base.order<<"\n\n";
								//it->dst = dst;
								s.erase(*it);
								s.insert(for_verify_LR1(index, a_symbol.name, dst));

								action_struct* new_action = new action_struct(a_symbol.name, dst);
								//link this new unit to the index-th position of LR1 map

								//insert into the head!
								action_struct* tmp = LR1_table.at(index).action_ptr->next;
								new_action->next = tmp;
								LR1_table.at(index).action_ptr->next = new_action;


								//move in enforcily


								//break;
							}

						}
					}
				}
			}

			if (item_unit.base.left.name == "start" && item_unit.index == 1)
			{
				//here we judge the prospect symbols
				if (item_unit.prospect_symbol=="#")
				{
					//for (const auto& i : item_unit.prospect_symbols)
					std::string i = "#";
					{
						if (i == "#")
						{

							auto  it = s.find(for_verify_LR1(index, "#", ACC));
							if (it == s.end())
							{
								s.insert(for_verify_LR1(index, "#", ACC));

								action_struct* new_action = new action_struct("#", ACC);
								//link this new unit to the index-th position of LR1 map

								//insert into the head!
								action_struct* tmp = LR1_table.at(index).action_ptr->next;
								new_action->next = tmp;
								LR1_table.at(index).action_ptr->next = new_action;
							}
							else
							{
								if (it->dst != ACC)
								{
									std::cerr << "THIS IS NOT LR1 GRAMMER!" << index << " " << "#" << "\n";
									break;
								}

							}

							/*
							action_struct* new_action = new action_struct("#", ACC);// r project
							action_struct* tmp = LR1_table.at(index).action_ptr->next;
							new_action->next = tmp;
							LR1_table.at(index).action_ptr->next = new_action;

                            const auto& it = s.find(for_verify_LR1(index, "#"));
							if (it == s.end())
								s.insert(*it);
							else
								std::cerr << "THIS IS NOT LR1 GRAMMER!\n";*/
						}
					}
					
				}
			}
			else
			{

				//r 2rd principle
				if (item_unit.index == item_unit.base.right_list.size())
				{
					if (item_unit.base.right_list.at(0).name != "$")
					{
						//for (const auto& symbol_unit : item_unit.prospect_symbols)
						auto symbol_unit = item_unit.prospect_symbol;
						{

							auto  it = s.find(for_verify_LR1(index, symbol_unit, item_unit.base.order));
							if (it == s.end())
							{
								s.insert(for_verify_LR1(index, symbol_unit, item_unit.base.order));

								action_struct* new_action = new action_struct(symbol_unit,item_unit.base.order);
								//link this new unit to the index-th position of LR1 map

								//insert into the head!
								action_struct* tmp = LR1_table.at(index).action_ptr->next;
								new_action->next = tmp;
								LR1_table.at(index).action_ptr->next = new_action;
							}
							else
							{
								if (it->dst != item_unit.base.order)
								{
									std::cerr << "THIS IS NOT LR1 GRAMMER!" << index << " " << symbol_unit << "\n";
									break;
								}

							}
							/*
							action_struct* new_action = new action_struct(symbol_unit, item_unit.base.order);// r project
							action_struct* tmp = LR1_table.at(index).action_ptr->next;
							new_action->next = tmp;
							LR1_table.at(index).action_ptr->next = new_action;

							/*const auto& it = s.find(for_verify_LR1(index, symbol_unit));
							if (it == s.end())
								s.insert(*it);
							else
								std::cerr << "THIS IS NOT LR1 GRAMMER!\n";*/
						}
						/*
						for (const auto& symbol_unit : terminators)
						{
							action_struct* new_action = new action_struct(symbol_unit.name, item_unit.base.order);// r project
							action_struct* tmp = LR1_table.at(index).action_ptr->next;
							new_action->next = tmp;
							LR1_table.at(index).action_ptr->next = new_action;
						}*/
					}
				}
				// speciallly cope with $
				if (item_unit.base.right_list.at(0).name == "$")
				{
					//for (const auto& symbol_unit : item_unit.prospect_symbols)
					auto symbol_unit = item_unit.prospect_symbol;
					{

						auto  it = s.find(for_verify_LR1(index, symbol_unit, item_unit.base.order));
						if (it == s.end())
						{
							s.insert(for_verify_LR1(index, symbol_unit, item_unit.base.order));

							action_struct* new_action = new action_struct(symbol_unit, item_unit.base.order);
							//link this new unit to the index-th position of LR1 map

							//insert into the head!
							action_struct* tmp = LR1_table.at(index).action_ptr->next;
							new_action->next = tmp;
							LR1_table.at(index).action_ptr->next = new_action;
						}
						else
						{
							if (it->dst != item_unit.base.order)
							{
								std::cerr << "THIS IS NOT LR1 GRAMMER!" << index << " " << symbol_unit << "\n";
								break;
							}

						}

						/*
						action_struct* new_action = new action_struct(symbol_unit, item_unit.base.order);// r project
						action_struct* tmp = LR1_table.at(index).action_ptr->next;
						new_action->next = tmp;
						LR1_table.at(index).action_ptr->next = new_action;

						/**const auto& it = s.find(for_verify_LR1(index, symbol_unit));
						if (it == s.end())
							s.insert(*it);
						else
							std::cerr << "THIS IS NOT LR1 GRAMMER!\n";*/
					}
				}
			}
			
			/*
			auto ite = go_map.at(index).find(item_unit.base.left.name);
			if (ite != go_map.at(index).end())
			{
				int dst = ite->second;
				goto_struct* new_goto = new goto_struct(item_unit.base.left.name,dst);// goto project
				goto_struct* tmp = LR1_table.at(index).goto_ptr->next;
				new_goto->next = tmp;
				LR1_table.at(index).goto_ptr->next = new_goto;

			}
			*/
			
		}
		for (const auto& i : non_terminators)
		{
			auto ite = go_map.at(index).find(i.name);
			if (ite != go_map.at(index).end())
			{


				int dst = ite->second;

				auto  it = s.find(for_verify_LR1(index, i.name, dst));
				if (it == s.end())
				{
					s.insert(for_verify_LR1(index, i.name, dst));

					goto_struct* new_goto = new goto_struct(i.name, dst);
					//link this new unit to the index-th position of LR1 map

					//insert into the head!
					goto_struct* tmp = LR1_table.at(index).goto_ptr->next;
					new_goto->next = tmp;
					LR1_table.at(index).goto_ptr->next = new_goto;
				}
				else
				{
					if (it->dst != dst)
					{
						std::cerr << "THIS IS NOT LR1 GRAMMER!" << index << " " << i.name<< "\n";
						break;
					}

				}

				/*
				goto_struct* new_goto = new goto_struct(i.name, dst);// goto project
				goto_struct* tmp = LR1_table.at(index).goto_ptr->next;
				new_goto->next = tmp;
				LR1_table.at(index).goto_ptr->next = new_goto;
				*/

			}
		}
	}
	make_cache();
}

void Parser ::print_LR1_table()
{
	std::fstream of;
	of.open("./LR1_table_c.txt", std::ios::out);
	for (unsigned int i = 0; i < LR1_table.size(); i++)
	{
		// traverse 2 link lists
		of << std::setw(5) << i << "\n";
		of << "ACTION\n";
		action_struct* track = LR1_table.at(i).action_ptr->next;
		while (track != nullptr)
		{
			of << track->symbol_name << " ";
			if (track->dst < 0)//s
				of << "s" << -1 * track->dst;
			else
				of << "r" << 1 * track->dst;
			of << "  ||   ";
			track = track->next;
		}

		of << "\nGOTO\n";

		goto_struct* track2 = LR1_table.at(i).goto_ptr->next;
		while (track2 != nullptr)
		{
			of << track2->symbol_name << " ";
			//if (track->dst < 0)//s
				//of << "s" << -1 * track->dst;
			//else
				of << 1 * track2->dst;
			of << "  ||   ";
			track2 = track2->next;
		}
		of << "\n\n";
	}
}

void Parser::make_cache()
{
	std::fstream of("cache", std::ios::out);
	of << LR1_table.size() << "\n";
	for (unsigned int i = 0; i < LR1_table.size(); i++)
	{
		of << "action\n";
		action_struct* track= LR1_table.at(i).action_ptr->next;
		while (track != nullptr)
		{
			of << track->symbol_name << " "<<track->dst<<"\n";
			track = track->next;
		}

		of << "goto\n";

		goto_struct* track2 = LR1_table.at(i).goto_ptr->next;
		while (track2 != nullptr)
		{
			of << track2->symbol_name << " "<< 1 * track2->dst<<"\n";
			track2 = track2->next;
		}
		of << "\n";
	}
	of.close();
}

int Parser::read_cache()
{
	std::fstream f("cache", std::ios::in);
	std::string tmp;
    unsigned int size;

	std::string name;
	int dst;
    int tag=false;
	if (f.is_open())
	{
        tag=true;
		getline(f,tmp);
		std::stringstream sstrem(tmp);
		sstrem >> size;
		std::cout << "size'" << size << "\n";
		for (unsigned int i = 0; i < size; i++)
		{
			movement mov;
			action_struct* track1 = mov.action_ptr;
			goto_struct* track2 = mov.goto_ptr;

			getline(f, tmp); //readin the "action"
			getline(f, tmp);
			while(tmp!="goto")
			{
				std::stringstream strs(tmp);
				
				strs >> name;
				strs>> dst;
				action_struct* new_action = new action_struct(name,dst);
				track1->next = new_action;
				track1 = new_action;
				getline(f, tmp);
		
			}

			getline(f, tmp);
			while (tmp.empty() == false)
			{
				std::stringstream strs(tmp);

				strs >> name;
				strs >> dst;
				goto_struct* new_goto = new goto_struct(name, dst);
				track2->next = new_goto;
				track2 = new_goto;
				getline(f, tmp);
			}

			LR1_table.emplace_back(mov);
		}
    }
    f.close();

    return tag;
}

//we use the following command to generate the pic
//neato -Tpng D://compilationprinciple/parser/parser/DFA.dot -o D://compilationprinciple//parser//parser//DFA.png
void Parser::print_DFA()
{
    std::fstream of;
    of.open("./DFA.dot", std::ios::out);

    of << "digraph G{\n";

    of << "graph[dpi=500, autosize=false,size=\"150,150\"];\n";

    of << "overlap=false;\nspines=true;\n";

    of << "node [shape=box];\n";
    of<<"edge[lblstyle = \"above, sloped\"];\n";

    for (const auto& unit : _item_groups_)
    {
        of << "node" << unit.id << "[label=\" ";

        //write the items into a node
        /*
        for (auto item_unit : unit.items)
        {
            of << item_unit.base.left.name << " -> ";
            for (unsigned int i = 0; i < item_unit.base.right_list.size(); i++)
            {
                if (i == item_unit.index)
                    of << ".";
                if (item_unit.base.right_list.at(i).name != "$")
                    of << item_unit.base.right_list.at(i).name;
            }
            of << "  ";

            //prospect symbol
            for (auto prospect_unit : item_unit.prospect_symbols)
                of << prospect_unit << " | ";
            of << "\\n";
        }
        */
        of << unit.id;
        of << "\"]";
        of << "\n";
    }

    //here we start to output the relationships between item_groups
    of << "\n";

    int cnt = 0;
    for (const auto item_group_map : go_map)
    {
        for (const auto& go_relation : item_group_map)
        {
            std::string label = go_relation.first;
            of << "node" << cnt << "->" << "node" << go_relation.second << "\n ";// [label = \"" << label << "\",constraint=false]\n";
        }
    }
    of << "}";
    of.close();
}

//semantic action
bool _IsDigit(const string a)
{
    for (auto it : a)
        if (!isdigit(it))
            return false;
    return true;
}


//TODO: MEND this function
std::tuple<bool, std::string, int, int> Parser::check(const std::string path)
{
    //track the max number of stack
      // 1
    unsigned int max_num = 0;

    int nodeID = 0;
    int eplisonID = 2147483647;
    std::fstream f;
    f.open(path, std::ios::in);
    std::fstream of;
    of.open("./grammer_tree.dot", std::ios::out);

    while (!itemS.empty())
        itemS.pop();
    while (!symbolS.empty())
        symbolS.pop();
    /////////////////////////////////
    itemS.push(0);// I0 into stack
    symbolS.push(word(-1, -1, "#", "#", -1));

    of << "digraph G{\ngraph[dpi=300,autosize=false,size=\"200, 200\"];\noverlap=false; \nspines=true;\nnode[ shape=\"box\"];\n";

    while (f.eof() == false)
    {
        bool findFLAG = false, R_flag = false;
        unsigned int pos = 0, pos1 = 0, pos2 = 0;
        std::string tmp;

        getline(f, tmp);
        pos = tmp.find_first_of(' ');
        pos1 = tmp.find_first_of(' ', pos + 1);
        pos2 = tmp.find_first_of(' ', pos1 + 1);

        word inputw(atoi(tmp.substr(0, pos - 0).c_str()), atoi(tmp.substr(pos + 1, pos1 - pos - 1).c_str()), tmp.substr(pos1 + 1, pos2 - pos1 - 1), tmp.substr(pos2 + 1, tmp.size() - pos2 - 1), nodeID);

        int topState = itemS.top();

        action_struct* a_ptr = LR1_table[topState].action_ptr->next;

        bool syntax_tag = false;////
        bool semantic_tag = true;////
        int error_line = 0, error_col = 0;////
        std::string error_message;
        while (a_ptr)
        {
            R_flag = false;
            if (a_ptr->symbol_name == inputw.value)
            {
                syntax_tag = true;
                if (a_ptr->dst == ACC)
                {
                    of << "}\n";

                    if(of.is_open())
                        of.close();
                    return std::make_tuple(true, "none", 0, 0);
                }
                findFLAG = true;
                if (a_ptr->dst < 0)
                {//s
                    itemS.push(-a_ptr->dst);
                    nodeID++;
                    inputw.setID(nodeID);

                    symbolS.push(inputw);

                    if (itemS.size() > max_num)
                        max_num = itemS.size();
                    break;
                }
                else if (a_ptr->dst > 0)
                {//r
                    syntax_tag = false;
                    generator x = pure_generator_list[a_ptr->dst];
                    nodeID++;
                    int fid = nodeID;

                    //word fword(LINEOFNONT, x.left.name, x.left.name, fid);
                    word fword;
                    std::string type;
                    bool isTrans = false;
                    of << "node" << nodeID << "[label=\"" << x.left.name << "\"]\n";
                    /*-----------------------------*/
                    if (x.right_list.at(0).name != "$")
                    {
                        int n = x.right_list.size();
                        for (int i = 0; i < n; i++)
                        {
                            word topW = symbolS.top();
                            
                            if (topW.line != LINEOFNONT)
                                of << "node" << topW.id << "[label=\"" << topW.value << "\\n" << topW.realV << "\"]\n";
                            of << "node" << fid << "->node" << topW.id << "\n";

                            symbolS.pop();
                            itemS.pop();

                        }
                    }
                    else
                    {
                        eplisonID--;
                        of << "node" << eplisonID << "[label=\"Eplison\"]\n";
                        of << "node" << fid << "->node" << eplisonID << "\n";
                    }/**/
                    topState = itemS.top();
                    
                    fword.setFid(fid);
                    fword.value = x.left.name;
                    symbolS.push(fword);

                    //goto
                    goto_struct* g_ptr = LR1_table[topState].goto_ptr->next;
                    while (g_ptr)
                    {
                        if (g_ptr->symbol_name == x.left.name)
                        {
                            syntax_tag = true;
                            R_flag = true;
                            itemS.push(g_ptr->dst);

                            break;
                        }
                        g_ptr = g_ptr->next;
                    }
                    if (R_flag == true)
                    {
                        topState = itemS.top();
                        a_ptr = LR1_table[topState].action_ptr->next;
                        continue;
                    }
                    break;
                }
            }
            a_ptr = a_ptr->next;
        }
        if (a_ptr == nullptr)
        {
            syntax_tag = false;
            if(of.is_open())
                of.close();
            return std::make_tuple(false, "SYNTAX ERROR::syntax error\n", inputw.line, inputw.col);
        }
        if (semantic_tag == false)
        {
            if(of.is_open())
                of.close();
            return std::make_tuple(false, error_message, error_line, error_col);
        }
        if (syntax_tag == false)
        {
            if(of.is_open())
                of.close();
            return std::make_tuple(false, "SYNTAX ERROR::syntax error\n", inputw.line, inputw.col);
        }
        if (findFLAG == false && R_flag == false) 
        {

            if(of.is_open())
                of.close();
            return std::make_tuple(false, "SYNTAX ERROR::syntax error\n", inputw.line, inputw.col);
        }
    }
}



