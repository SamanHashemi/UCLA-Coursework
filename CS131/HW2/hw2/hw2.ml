open List;;

type ('nonterminal, 'terminal) parse_tree =
  | Node of 'nonterminal * ('nonterminal, 'terminal) parse_tree list
  | Leaf of 'terminal

type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal

let acc_empty_suffix = function
	| _::_ -> None
	| x -> Some x;;


(* Number 1 *)
let rec get_rhs rules lhs = match rules with
	| hd::tl ->
		if (lhs == (fst hd)) then
			(snd hd) :: (get_rhs tl lhs)
		else
			(get_rhs tl lhs) 
	| [] -> [];;

let convert_grammar gram =
	(fst gram), (get_rhs (snd gram))
;;

(* Number 2 *)
let rec tree_helper tree_list = match tree_list with
	| [] -> [] 
	| h :: t -> match h with 
		| Leaf l -> l :: (tree_helper t)
		| Node (nt, tree) -> (tree_helper tree) @ (tree_helper t)
;;

let parse_tree_leaves tree =
	tree_helper [tree]
;;



let rec create tnode derv =
	let rec c_helper tlist derv =
	match tlist with
	| [] -> None
	| tnode::t -> match tnode with
						 | Node (x, y) -> (match (create (Node (x, y)) derv) with
						 				| Some z -> Some (z::t)
						 				| None -> (match (c_helper t derv) with
						 				  			 | Some z -> Some ((Node (x, y))::z)
						 							| None -> None))
						 | Leaf x -> (match (c_helper t derv) with
									  | None -> None
									  | Some y -> Some ((Leaf x)::y))
	in
	let node t_n =
		match t_n with
		| T t -> Leaf t
		| N n -> Node (n, []) in
	match tnode with
	| Node (x, y) -> (match y with
					 | y when y = [] -> Some (Node (x, map node derv))
					 | m -> (match (c_helper m derv) with
					 		| Some lst -> Some (Node (x, lst))
					 		| None -> None))
	| Leaf x -> None
;;


let rec matcher_help_list sym rlist prules acc rules frag = 
	let rec match_helper rule prules acc rules frag =
	(match rule with
	| rule_h::rule_t -> (match frag with
				
				| frag_h::frag_t -> (match rule_h with
									| N non_terminal -> matcher_help_list non_terminal (prules non_terminal) prules (match_helper rule_t prules acc) rules frag
									| T terminal -> (if terminal = frag_h 
														then match_helper rule_t prules acc rules frag_t
														else None, rules))
				| [] -> None, rules)
	| [] -> (acc rules frag))
	in
	match rlist with
	| [] -> None, rules
	| h::t -> 	match (create rules h) with 
				| Some x -> 
					(match (match_helper h prules acc x frag) with
					| (Some y, ur) -> (Some y, ur)
					| (None, _) -> (matcher_help_list sym t prules acc rules frag))
				| None -> (None, rules)
					;;

(* Number 3 *)
let make_matcher gram =
	let curr = (snd gram) (fst gram) in 
	let matcher acc frag = 
	let acceptor rules frag = (acc frag, rules) in
	(fst (matcher_help_list (fst gram) curr (snd gram) acceptor (Node ((fst gram), [])) frag))
	
	in 

	matcher;;

(* Number 4 *)
let make_parser gram =
	let curr_expr = (snd gram) (fst gram) in

	let get_matchrule acc frag =
	let acceptor rules frag = (acc frag, rules) in
	(matcher_help_list (fst gram) curr_expr (snd gram) acceptor (Node ((fst gram), [])) frag) in

	let parser frag = 
	let rules = (snd (get_matchrule acc_empty_suffix frag)) in
	let matcher = (fst (get_matchrule acc_empty_suffix frag)) in
	match matcher with
		| Some _ -> Some (rules)
		| None -> None
	in
	parser;;