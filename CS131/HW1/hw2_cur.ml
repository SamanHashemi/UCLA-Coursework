type ('nonterminal, 'terminal) symbol = N of 'nonterminal | T of 'terminal;;

type ('nonterminal, 'terminal) parse_tree =
  | Node of 'nonterminal * ('nonterminal, 'terminal) parse_tree list
  | Leaf of 'terminal

let left_recursion = false;;

(* Number 1 *)
let rec get_rhs rules lhs = match rules with
	| [] -> []
	| hd::tl ->
			if (lhs == (fst hd)) then
				(snd hd) :: (get_rhs tl lhs)
			else
				(get_rhs tl lhs)
;;

let convert_grammar gram =
	(fst gram), (get_rhs (snd gram))
;;

(* Number 2 *)
let parse_tree_leaves tree = match tree_list with
  		| [] -> [] 
  		| hd :: tl -> match hd with 
  			| Node (nt, tree) -> (parse_tree_leaves tree) @ (parse_tree_leaves tl)
  			| Leaf l -> l :: (parse_tree_leaves tl)
	in
	parse_tree_leaves [tree]
;;

(* Number 3 *)
let make_matcher gram accept frag = 
	let rule_func = snd gram in 
	produces_valid_match cur_rule accept cand_derivation frag) 
	and produces_valid_match cur_rule accept current_match frag = 
	match cur_rule with
		| [] -> accept current_match frag 
		| _ -> match frag with
			| [] -> None
			| prefix::suffix -> match cur_rule with 
				| (T term_sym)::t when prefix = term_sym -> produces_valid_match t accept current_match suffix
				| (T term_sym)::t -> None 
				| (N nonterm_sym)::t -> (fun rhs_rule_acceptor -> 
					match_symbol nonterm_sym rhs_rule_acceptor current_match frag (rule_func nonterm_sym))
					 (produces_valid_match t accept)
				| [] -> None in

	let start_symbol = fst gram and rules_for_start = rule_func (fst gram) in 
	match_symbol start_symbol accept [] frag rules_for_start;;

(* Number 4 *)
let rec option_iterator2 
	(symbols : ('nonterm,'term) symbol list) 
	(rules : ('nonterm -> ('nonterm, 'term) symbol list list)) 
	(frag : 'term list)
	(nonterm : 'nonterm) 
	(options : ('nonterm,'term) symbol list list) 
	func =
	match options with
	| [] -> None (* No more options, fail *)
	| opt_hd::opt_tl ->
		let result = func (opt_hd @ symbols) rules frag in
		match result with 
		| None ->
			option_iterator2 symbols rules frag nonterm opt_tl func 
		| Some a -> Some ((nonterm, opt_hd) :: a)
(* 
let rec get_derivation
	(symbols : ('nonterm,'term) symbol list) 
	(rules : ('nonterm -> ('nonterm, 'term) symbol list list)) 
	(frag : 'term list)
	: ('nonterm * ('nonterm,'term) symbol list) list option =

	if symbols = [] && frag = [] then Some [] else

	if symbols = [] then None else

	if (left_recursion && ((List.length symbols) > (List.length frag))) then None else

	match (List.hd symbols) with 
	| T term ->
		(match frag with
		| [] -> None
		| f_hd::f_tl -> 
			if term = f_hd then
				get_derivation (List.tl symbols) rules (List.tl frag)
			else
				None)

	| N nonterm -> 
		let options = rules nonterm in
		if options = [] then
			option_iterator2 (List.tl symbols) rules frag nonterm [[]] get_derivation
		else 
			option_iterator2 (List.tl symbols) rules frag nonterm options get_derivation
;; *)

let rec to_ptree 
	(deriv : ('nonterm * ('nonterm,'term) symbol list) list) 
	: ('nonterm * ('nonterm,'term) symbol list) list * ('nonterm,'term) parse_tree =

	let rec ptree_iterator 
		(deriv : ('nonterm * ('nonterm,'term) symbol list) list)
		(rhs : ('nonterm,'term) symbol list) 
		: ('nonterm * ('nonterm,'term) symbol list) list * ('nonterm,'term) parse_tree list =
			match rhs with 
			| [] -> deriv, []
			| r_hd::r_tl -> 
				match r_hd with
				| T term -> 
					let res = ptree_iterator deriv r_tl in
					(match res with | (remaining_deriv, subtrees) ->
						remaining_deriv, ((Leaf	term) :: subtrees))

				| N nonterm ->
					let expansion = to_ptree deriv in
					match expansion with | (remaining_deriv, cur_node) ->

					let res = ptree_iterator remaining_deriv r_tl in
					match res with | (remaining_deriv2, subtrees2) ->
						remaining_deriv2, (cur_node :: subtrees2)
	in

	match deriv with
	| d_hd::d_tl -> match d_hd with (lhs, rhs) ->
		let res = ptree_iterator d_tl rhs in
		match res with (remaining_deriv, subtrees) ->
			remaining_deriv, Node (lhs, subtrees)
;;

let parser_wrapper gram frag =
	let rec get_derivation
	(symbols : ('nonterm,'term) symbol list) 
	(rules : ('nonterm -> ('nonterm, 'term) symbol list list)) 
	(frag : 'term list)
	: ('nonterm * ('nonterm,'term) symbol list) list option =

	if symbols = [] && frag = [] then Some [] else

	if symbols = [] then None else

	if (left_recursion && ((List.length symbols) > (List.length frag))) then None else

	match (List.hd symbols) with 
	| T term ->
		(match frag with
		| [] -> None
		| f_hd::f_tl -> 
			if term = f_hd then
				get_derivation (List.tl symbols) rules (List.tl frag)
			else
				None)

	| N nonterm -> 
		let options = rules nonterm in
		if options = [] then
			option_iterator2 (List.tl symbols) rules frag nonterm [[]] get_derivation
		else 
			option_iterator2 (List.tl symbols) rules frag nonterm options get_derivation
	in
	let deriv = get_derivation [N (fst gram)] (snd gram) frag in
	match deriv with
	| None -> None
	| Some d -> 
		let res = to_ptree d in
		match res with | (junk, ptree) -> Some ptree
;;

let make_parser gram = 
	parser_wrapper gram
;;