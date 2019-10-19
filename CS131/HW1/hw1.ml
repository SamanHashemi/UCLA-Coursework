type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal

let rec subset a b =
    match a with
    [] -> true
    | h::t -> if List.mem h b then subset t b else false;;

let equal_sets a b = subset a b && subset b a;;

let rec set_union a b = match a with
    | [] -> b
    | h::t -> if List.exists (fun x -> x = h) b then set_union t b else set_union t (h::b);;

let set_intersection a b = List.find_all (fun x -> List.mem x a) b;;

let set_diff a b = List.filter (fun x -> List.for_all (fun elem -> elem <> x) b) a;;

let rec computed_fixed_point eq f x = if eq (f x) x then x else computed_fixed_point eq f (f x);;

let rhs (_, x) = x;;
let lhs (x, _) = x;;

let rec filter_reachable_rules start_nt grules =
    let rec get_nonterms rlist =
        match rlist with
        | [] -> []
        | N h::t -> (h)::(get_nonterms t)
        | T h::t -> get_nonterms t in
    let rec nonterm_parse cur_nonterms rt_list =
        match rt_list with
        | [] -> cur_nonterms
        | rules::tup_list -> 
        if (List.mem (lhs rules) cur_nonterms) 
            then nonterm_parse (set_union cur_nonterms (get_nonterms (rhs rules))) tup_list
        else nonterm_parse cur_nonterms tup_list in
    let rec all_nonterms cur_nonterms rt_list =
        let r1 = nonterm_parse cur_nonterms rt_list in
        let r2 = nonterm_parse r1 rt_list in
            if equal_sets r1 r2 then r1 else all_nonterms r2 rt_list in

    let rn = (all_nonterms [start_nt] grules) in
    List.filter (fun x -> List.mem (lhs x) rn) grules 
;;

let filter_reachable g = match g with
    | (start_nt, rules) -> (start_nt, filter_reachable_rules start_nt rules)
    | _ -> g
;;