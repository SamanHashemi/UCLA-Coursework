let rec subset a b = match a with 
	[] -> true
	|x::y -> if List.mem x b then subset y b else false;;

let equal_sets a b = (subset a b) && (subset b a);;

let rec set_union a b = match a with
	[] -> List.sort_uniq compare b
	|x::y -> if List.mem x b then set_union y b else (set_union y (x::b));;

let rec set_intersection a b = match a with
	[] -> []
	|x::y -> if List.mem x b then x::(set_intersection y b) else set_intersection y b;;

let rec set_diff a b = match a with 
	[] -> []
	|x::y -> if List.mem x b then set_diff y b else x::(set_diff y b);;

let rec computed_fixed_point eq f x = 
	if eq (f x) x then x else computed_fixed_point eq f (f x);;

let rec computed_periodic_point eq f p x = 
	let rec funcLoop g y q = match q with
		0 -> y
		| _ -> funcLoop g (g y) (q-1) in
	match p with
	0 -> x
	|_ -> if eq (funcLoop f x p) x then x else computed_periodic_point eq f (p) (f x);;

let rec while_away s p x = 
	if not (p x) then [] else x::(while_away s p (s x));;

let rec rle_decode lp = 
	let rec funcLoop a b = match a with
		0 -> []
		|_ -> b::funcLoop (a-1) b in
	match lp with 
	[] -> []
	| x::y -> match x with 
		(a, b) -> (funcLoop a b)@(rle_decode y);;

type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal ;;

 let rec safe rules safe_rules = 
	 let is_terminal sym safe_rules = match sym with 
 	  | N e -> List.mem e safe_rules
 	  | T _ -> true in 
	    match rules with
 		| [] -> true
 		| h::t -> if is_terminal h safe_rules then (safe t safe_rules) 
 				else false;;

let rec add_safe_rules_one_pass safe_rules curr_rules = match curr_rules with
 | [] ->  safe_rules
 | h::t -> if not(List.mem (fst h) safe_rules) && (safe (snd h) safe_rules)
                        then (add_safe_rules_one_pass ((fst h)::safe_rules) t)
                        else (add_safe_rules_one_pass safe_rules t);;

let make_safe_list (safe_rules, orig_rules) =
	(add_safe_rules_one_pass safe_rules orig_rules), orig_rules

let is_equal (a,b) (x,y) = equal_sets a x;;

let rec filter safe_rules orig_rules = match orig_rules with
| [] -> []
| h::t -> 	if (safe (snd h) safe_rules) then h::(filter safe_rules t)
			else (filter safe_rules t);;


let filter_blind_alleys g = 
	let rules = (snd g) in
	(fst g), (filter (fst (computed_fixed_point 
		is_equal make_safe_list ([], rules))) rules);;
