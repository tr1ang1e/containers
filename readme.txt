
commit      commit
number      description    

    1       + thread-safety queue implimented
            + list_logic.c = linux kermel list ( <list.h> ) 

    2       + red-black tree interface defined
            + some red-black tree private functions implemented     

    3       + most of red-black tree interface functions implemented
            + most of red-black tree private functions implemented   
            + .clang-format: 'BreakBeforeBraces: Allman'

    4       + red-black tree interface is done
            + approach of finding nodes is changed 
            + red-black tree private functions implemented  
            + .clang-format: 'AlwaysBreakAfterReturnType: None'

    5       + red-black::rbt_delete_black_node() excess nodes removed
            + rb_tree.c syntax bugs fixed

    6       + conditional variables to thread-safety queue    
            + tests for queue    

    7       + tests for red-black tree (insert, get)

current     >
		>           

todo        - tests for red-black tree (delete, destroy)
		- list implementation (+ to format.sh)
            - tests for list
            
            ? make red-black tree thread-safety   