#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#define defaultorder 4
#define bool char
#define false 0
#define true 1

typedef struct record {
    int value;
} record;

typedef struct node {
    char leaf_or_not;
    int number_of_keys;
    void ** ptrs;
    int * keys;
    struct node * parent;
    struct node * next;
} node;

int order ;
node * delete_entry( node *, node *, int, void * );
node* insert_into_parent(node *, node *, int, node *);
int exact_search(node * root, int key);
int range_search(node *root);
// int batch_search(node * root,int n, int key);
record* newrecord(int value) {
    record * new_record = (record *)malloc(sizeof(record));
    if (new_record == NULL) {
	perror("Record creation.");
	exit(EXIT_FAILURE);
    }
    else {
	new_record->value = value;
    }
    return new_record;
}

//-------------------------------- _FINDING_LEAF_Node_VALUE------------------------//
node* find_leaf( node * root, int key) {
    int i = 0;
    node *c = root;
    if (c == NULL) {
	   return c;
    }
    while (!c->leaf_or_not)
    {
    	i = 0;
    	    while (i < c->number_of_keys) {
    	    if (key >= c->keys[i]) i++;
    	    else break;
    	    }
	    c = (node *)c->ptrs[i];
    }
    return c;
}

record *find(node * root, int key) {
    int i = 0;
    node *c = find_leaf( root, key);
    if (c == NULL) return NULL;
    for (i = 0; i<c->number_of_keys; i++)
	if (c->keys[i] == key) break;
    if (i == c->number_of_keys)
	return NULL;
    else
	return (record *)c->ptrs[i];
}


node * newnode( void ) {
    node * new_node;
    new_node = (node*)malloc(sizeof(node));

    new_node->keys = (int*)malloc( (order - 1) * sizeof(int) );

    new_node->ptrs = (void**)malloc( order * sizeof(void *) );

    new_node->leaf_or_not = 0;
    new_node->number_of_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

node * insert_at_leaf( node * leaf, int key, record * pointer ) {

    int i, insertpoint;

    insertpoint = 0;
    while (insertpoint < leaf->number_of_keys && leaf->keys[insertpoint] < key)
	insertpoint++;

    for (i = leaf->number_of_keys; i > insertpoint; i--) {
	leaf->keys[i] = leaf->keys[i - 1];
	leaf->ptrs[i] = leaf->ptrs[i - 1];
    }
    leaf->keys[insertpoint] = key;
    leaf->ptrs[insertpoint] = pointer;
    leaf->number_of_keys++;
    return leaf;
}

node * insert_into_node(node * root, node * n,int left_index, int key, node * right) {
    int i;

    for (i = n->number_of_keys; i > left_index; i--) {
	n->ptrs[i + 1] = n->ptrs[i];
	n->keys[i] = n->keys[i - 1];
    }
    n->ptrs[left_index + 1] = right;
    n->keys[left_index] = key;
    n->number_of_keys++;
    return root;
}

node * insert_into_node_after_splitting(node * root, node * old_node, int left_index,
	int key, node * right) {

    int i, j, s, k_prime;
    node * new_node, * child;
    int * temp_keys;
    node ** temp_ptrs;

    temp_ptrs =(node**) malloc( (order + 1) * sizeof(node *) );

    temp_keys = (int*)malloc( order * sizeof(int) );

    for (i = 0, j = 0; i < old_node->number_of_keys + 1; i++, j++) {
	if (j == left_index + 1) j++;
	temp_ptrs[j] = (node*)old_node->ptrs[i];
    }

    for (i = 0, j = 0; i < old_node->number_of_keys; i++, j++) {
	if (j == left_index) j++;
	temp_keys[j] = old_node->keys[i];
    }

    temp_ptrs[left_index + 1] = right;
    temp_keys[left_index] = key;

    if(order%2==0)
      s=order/2;
    else
    s=order/2+1;

    new_node = newnode();

    old_node->number_of_keys = 0;
    for (i = 0; i < s - 1; i++) {
	old_node->ptrs[i] = temp_ptrs[i];
	old_node->keys[i] = temp_keys[i];
	old_node->number_of_keys++;
    }
    old_node->ptrs[i] = temp_ptrs[i];
    k_prime = temp_keys[s - 1];
    for (++i, j = 0; i < order; i++, j++) {
	new_node->ptrs[j] = temp_ptrs[i];
	new_node->keys[j] = temp_keys[i];
	new_node->number_of_keys++;
    }
    new_node->ptrs[j] = temp_ptrs[i];

    new_node->parent = old_node->parent;
    for (i = 0; i <= new_node->number_of_keys; i++) {
	child =(node*) new_node->ptrs[i];
	child->parent = new_node;
    }


    return insert_into_parent(root, old_node, k_prime, new_node);
}

node* insert_into_parent(node * root, node * left, int key, node * right)
{

    int left_index;
    node * parent;

    parent = left->parent;

    if (parent == NULL)
	{
	node * r = newnode();
	r->keys[0] = key;
	r->ptrs[0] = left;
	r->ptrs[1] = right;
	r->number_of_keys++;
	r->parent = NULL;
	left->parent = r;
	right->parent = r;
	return r;
	}

	left_index=0;

	while (left_index <= parent->number_of_keys &&parent->ptrs[left_index] != left)
	{left_index++;}

    if (parent->number_of_keys < (order - 1))
	return insert_into_node(root, parent, left_index, key, right);

    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}

node * split(node * root, node * leaf, int key, record * pointer)
   {
    node * leaf_s;
    int * newkeys;
    void ** newptrs;
    int insertindex, s, new_key, i, j;

    leaf_s = newnode();
    leaf_s->leaf_or_not=1;

    newkeys = (int*)malloc( order * sizeof(int) );

    newptrs = (void**)malloc( order * sizeof(void *) );

    insertindex = 0;
    while (insertindex < order - 1 && leaf->keys[insertindex] < key)
	insertindex++;

    for (i = 0, j = 0; i < leaf->number_of_keys; i++, j++) {
	if (j == insertindex) j++;
	newkeys[j] = leaf->keys[i];
	newptrs[j] = leaf->ptrs[i];
    }

    newkeys[insertindex] = key;
    newptrs[insertindex] = pointer;

    leaf->number_of_keys = 0;

    if((order-1)%2==0)
      s = (order - 1)/2;
    else
      s = ((order - 1)/2)+1;

    for (i = 0; i < s; i++) {
	leaf->ptrs[i] = newptrs[i];
	leaf->keys[i] = newkeys[i];
	leaf->number_of_keys++;
    }

    for (i = s, j = 0; i < order; i++, j++) {
	leaf_s->ptrs[j] = newptrs[i];
	leaf_s->keys[j] = newkeys[i];
	leaf_s->number_of_keys++;
    }


    leaf_s->ptrs[order - 1] = leaf->ptrs[order - 1];//node pointed by last pointer now should be pointed by new node
    leaf->ptrs[order - 1] = leaf_s;//new node should be now pointed by previous node

    for (i = leaf->number_of_keys; i < order - 1; i++)//key holes in a node
	leaf->ptrs[i] = NULL;
    for (i = leaf_s->number_of_keys; i < order - 1; i++)
	leaf_s->ptrs[i] = NULL;//pointer holes in a node

    leaf_s->parent = leaf->parent;
    new_key = leaf_s->keys[0];

    return insert_into_parent(root, leaf, new_key, leaf_s);
}

node *insert( node * root, int key, int value ) {
    record *pointer;
    node *leaf;

    if (root == NULL)
       {
	node * l = newnode();

	l->leaf_or_not = 1;
	root = l;
	root->keys[0] = key;
	root->ptrs[0] = pointer;
	root->ptrs[order - 1] = NULL;
	root->parent = NULL;
	root->number_of_keys++;
	//printf("\nroot key[0] = %d",root->keys[0]);
	return root;
       }

    if (find(root, key) != NULL)
	return root;
    pointer = newrecord(value);

    leaf = find_leaf(root, key);

    if (leaf->number_of_keys < order - 1) {
	leaf = insert_at_leaf(leaf, key, pointer);
	return root;
    }

    return split(root, leaf, key, pointer);
}

int path_to_root( node * root, node * child ) {
    int length = 0;
    node * c = child;
    while (c != root) {
	c = c->parent;
	length++;
    }
    return length;
}

node *queue=NULL;

void enq( node * new_node ) {
    node * c;
    if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	    }
    	else {
		c = queue;
			while(c->next != NULL) {
	    	c = c->next;
			}
		c->next = new_node;
		new_node->next = NULL;
    }
}

node * dq( void ) {
    node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}
void printtree( node * root ) {

    node * n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL) {
	printf("\n Empty tree.\n");
	return;
    }

    queue = NULL;
    enq(root);
    while( queue != NULL ) 
    {
		n = dq();
			if (n->parent != NULL && n == n->parent->ptrs[0]) 
			{
			    new_rank = path_to_root( root, n );
			    if (new_rank != rank) 
			    {
				rank = new_rank;
				printf("\n");
			    }
			}

		for (i = 0; i < n->number_of_keys; i++) 
		{
		    printf("%d ", n->keys[i]);
		}
		if (!n->leaf_or_not){
		    for (i = 0; i <= n->number_of_keys; i++)
			enq((node*)n->ptrs[i]);
			}
	printf(" | ");
    }
    printf("\n");
}

int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}

node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index,
        int k_prime_index, int k_prime) {

    int i;
    node * tmp;

    if (neighbor_index != -1) {
        if (!n->leaf_or_not)
            n->ptrs[n->number_of_keys + 1] = n->ptrs[n->number_of_keys];
        for (i = n->number_of_keys; i > 0; i--) {
            n->keys[i] = n->keys[i - 1];
            n->ptrs[i] = n->ptrs[i - 1];
        }
        if (!n->leaf_or_not) {
            n->ptrs[0] = neighbor->ptrs[neighbor->number_of_keys];
            tmp = (node *)n->ptrs[0];
            tmp->parent = n;
            neighbor->ptrs[neighbor->number_of_keys] = NULL;
            n->keys[0] = k_prime;
            n->parent->keys[k_prime_index] = neighbor->keys[neighbor->number_of_keys - 1];
        }
        else {
            n->ptrs[0] = neighbor->ptrs[neighbor->number_of_keys - 1];
            neighbor->ptrs[neighbor->number_of_keys - 1] = NULL;
            n->keys[0] = neighbor->keys[neighbor->number_of_keys - 1];
            n->parent->keys[k_prime_index] = n->keys[0];
        }
    }

}
node * merge_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime) {

    int i, j, neighbor_insertion_index, n_start, n_end, new_k_prime;
    node * tmp;
    bool split;
    if (neighbor_index == -1) {
        tmp = n;
        n = neighbor;
        neighbor = tmp;
    }
    neighbor_insertion_index = neighbor->number_of_keys;


    split = false;


    if (!n->leaf_or_not) {
        neighbor->keys[neighbor_insertion_index] = k_prime;
        neighbor->number_of_keys++;


        n_end = n->number_of_keys;
        n_start = 0; // Only used in this special case.
        if (n->number_of_keys + neighbor->number_of_keys >= order) {
            split = true;
            n_end = cut(order) - 2;
        }

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->ptrs[i] = n->ptrs[j];
            neighbor->number_of_keys++;
            n->number_of_keys--;
            n_start++;
        }


        neighbor->ptrs[i] = n->ptrs[j];

        if (split) {
            new_k_prime = n->keys[n_start];
            for (i = 0, j = n_start + 1; i < n->number_of_keys; i++, j++) {
                n->keys[i] = n->keys[j];
                n->ptrs[i] = n->ptrs[j];
            }
            n->ptrs[i] = n->ptrs[j];
            n->number_of_keys--;
        }
        for (i = 0; i < neighbor->number_of_keys + 1; i++) {
            tmp = (node *)neighbor->ptrs[i];
            tmp->parent = neighbor;
        }
    }

    else {
        for (i = neighbor_insertion_index, j = 0; j < n->number_of_keys; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->ptrs[i] = n->ptrs[j];
            neighbor->number_of_keys++;
        }
        neighbor->ptrs[order - 1] = n->ptrs[order - 1];
    }

    if (!split) {
        root = delete_entry(root, n->parent, k_prime, n);
        free(n->keys);
        free(n->ptrs);
        free(n);
    }
    else
        for (i = 0; i < n->parent->number_of_keys; i++)
            if (n->parent->ptrs[i + 1] == n) {
                n->parent->keys[i] = new_k_prime;
                break;
            }

    return root;

}

node * remove_entry_from_node(node * n, int key, node * pointer) {

    int i, num_ptrs;
    i = 0;
    while (n->keys[i] != key)
        i++;
    for (++i; i < n->number_of_keys; i++)
        n->keys[i - 1] = n->keys[i];
    num_ptrs = n->leaf_or_not ? n->number_of_keys : n->number_of_keys + 1;
    i = 0;
    while (n->ptrs[i] != pointer)
        i++;
    for (++i; i < num_ptrs; i++)
        n->ptrs[i - 1] = n->ptrs[i];


   n->number_of_keys--;
    if (n->leaf_or_not)
        for (i = n->number_of_keys; i < order - 1; i++)
            n->ptrs[i] = NULL;
    else
        for (i = n->number_of_keys + 1; i < order; i++)
            n->ptrs[i] = NULL;

    return n;
}

node * adjust_root(node * root) {

    node * new_root;

    if (root->number_of_keys > 0)
        return root;

    if (!root->leaf_or_not) {
        new_root =(node*) root->ptrs[0];
        new_root->parent = NULL;
    }
    else
        new_root = NULL;

    free(root->keys);
    free(root->ptrs);
    free(root);

    return new_root;
}
int get_neighbor_index( node * n ) {

    int i;
    for (i = 0; i <= n->parent->number_of_keys; i++)
        if (n->parent->ptrs[i] == n)
            return i - 1;

    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n", (unsigned long)n);
    exit(EXIT_FAILURE);
}
node * delete_entry( node *root, node * n, int key, void * pointer ) {

    int min_keys;
    node * neighbor;
    int neighbor_index;
    int k_prime_index, k_prime;
    int capacity;

    n = remove_entry_from_node(n, key,(node*) pointer);
    if (n == root)
        return adjust_root(root);
    min_keys = n->leaf_or_not ? cut(order - 1) : cut(order) - 1;


    if (n->number_of_keys >= min_keys)
        return root;

    neighbor_index = get_neighbor_index( n );
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = n->parent->keys[k_prime_index];
    neighbor = neighbor_index == -1 ? n->parent->ptrs[1] :
        n->parent->ptrs[neighbor_index];

    capacity = n->leaf_or_not ? order : order - 1;
    if (neighbor->number_of_keys + n->number_of_keys < capacity)
        return merge_nodes(root, n, neighbor, neighbor_index, k_prime);
    else
        return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}

node * deletee (node * root, int key) {
    node * key_leaf;
    record * key_record;

    key_record = find(root, key);
    key_leaf = find_leaf(root, key);
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key, key_record);
        free(key_record);
    }
    return root;
}

int exact_search(node * root, int key){
	int i = 0,exact_match_flag=0;
	//-------------------------first find in leaf node is the key is found.---------
    node *c = find_leaf( root, key);
    	if (c == NULL) {
    		exact_match_flag=0;     //not found ;
    	}
    for (i = 0; i<c->number_of_keys; i++)
	{
		if (c->keys[i] == key){ 
			exact_match_flag=1;	 	//Found KEY ; 
			break; 
		}
	}
	return exact_match_flag;
}
int range_search(node *root){
	printf("Enter");

	int max,min,flag=1;
	node * n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;
	int exact_match_flag=0;
	scanf("%d", &min);
	scanf("%d", &max);
//----------------------------------
    queue = NULL;
    enq(root);
    while( queue != NULL ) 
    {
		n = dq();
			if (n->parent != NULL && n == n->parent->ptrs[0]) 
			{
			    new_rank = path_to_root( root, n );
			    if (new_rank != rank) 
			    {
				rank = new_rank;
				printf("Level Traversed %d", rank);
				printf("\n");
			    }
			}

		for (i = 0; i < n->number_of_keys; i++) 
		{
			if (n->leaf_or_not && n->keys[i]>=min && n->keys[i]<=max)
			{
				if(flag){ printf("Leaf NODE Traversed Neighbor\n"); flag=0;}
		    	printf("%d ",n->keys[i]);
			}
		}
		if (!n->leaf_or_not){
		    for (i = 0; i <= n->number_of_keys; i++)
			enq((node*)n->ptrs[i]);
			}
		// if (n->leaf_or_not && n->keys[i]>=min && n->keys[i]<=max)
		// 	{	
		// 	printf(" | ");
		// 	}
    }
	return 0;
}


int main()
{
	int c,v,find_key,exact_match,batch_search_value[100],n,i=0,max,min;
	node *root;
	root=NULL;

	printf("\n Order of B+ Tree : ");
	scanf("%d",&order);
	printf("\n ------------------------");
	printf("\n Operations :            \n");
	printf("\n   1  .  Insert          \n");
	printf("\n   2  .  Delete          \n");
	printf("\n   3  .  Exact MATCH     \n");
	printf("\n   4  .  Batch SEARCH    \n");
    printf("\n\n ------------------------");

	do
	{
		printf("\n\n Operation No : ");
		scanf("%d",&c);

		switch(c){
		case 1:printf("\n\nEnter key value :");
			scanf("%d",&v);
			root=insert(root,v,v);
			printf("\n B+ Tree : \n\n");
			printtree(root);
			break;

		case 2:
		    printtree(root);
		    printf("\n Delete Value : ");
		    scanf("%d", &v);
            root = deletee(root, v);
            printf("\n B+ Tree : \n\n");
			printtree(root);
            break;   
		case 3:
		    printf("\n Exact Value Search : ");
		    scanf("%d", &v);
			exact_match=exact_search(root,v);
			if (exact_match)
			{
				printf("Found key %d",v);
    		}else{
    			printf("Not found key %d",v);
    		}
            break;
        case 4:
		    printf("\n Batch Search : ");
		    printf("Enter the number of value u want to find :");
		    scanf("%d",&n);
		    for(i=0;i<n;i++){
				scanf("%d",&batch_search_value[i]);		    	
		    }
		    for (int i = 0; i < n; ++i)
		    {
   				exact_match=exact_search(root,batch_search_value[i]);
   				if (exact_match)
				{
					printf("Found key %d  \n",batch_search_value[i]);
    			}else{
    				printf("Not found key %d \n",batch_search_value[i]);
    			}
		    }
            break;
        case 5:
        	printf("\n Range Search : ");
		    printf("Enter the Range of value u want to find :");
		  
		    // for (int i = min; i < max; ++i)
		    // {
		    // 	exact_match=0;
   				exact_match=range_search(root);
   	// 			if (exact_match)
				// {
				// 	printf("Found key %d  \n",i);
    // 			}
		    // }
        	break;    
		case 6:
		    return 0;
		default:printf("\nwrong choice.........");
		}
	}while(c!=4);

	return 0;
}

