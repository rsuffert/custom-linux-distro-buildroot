/*
 * SSTF IO Scheduler
 *
 * For Kernel 4.13.9
 */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kernel.h>

/* SSTF data structure. */
struct sstf_data {
	struct list_head queue;
};

int last_sector_read = 0;
#ifdef ENABLE_ACCUM
int add_count,dsp_count=0;
#endif

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

/* Esta função despacha o próximo bloco a ser lido. */
static int sstf_dispatch(struct request_queue *q, int force){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction;
	struct request *closest_rq, *current_rq;
	unsigned long closest_distance, current_distance;
	#ifdef ENABLE_ACCUM
	if(add_count < 32){
		return 0;
	}
	#endif
	// assume closest request is the first one
	closest_rq = list_first_entry_or_null(&nd->queue, struct request, queuelist);
	if (!closest_rq) return 0;
	closest_distance = abs(last_sector_read - blk_rq_pos(closest_rq));

	// check if any other request is closer
	list_for_each_entry(current_rq, &nd->queue, queuelist) {
		current_distance = abs(last_sector_read - blk_rq_pos(current_rq));
		if (current_distance < closest_distance) {
			closest_rq = current_rq;
			closest_distance = current_distance;
		}
	}

	// dispatch closest request
	list_del_init(&closest_rq->queuelist);
	direction = (last_sector_read > blk_rq_pos(closest_rq)) ? 'R' : 'L';
	last_sector_read = blk_rq_pos(closest_rq);
	elv_dispatch_sort(q, closest_rq);
	#ifdef ENABLE_ACCUM
	dsp_count++;
	#endif
	printk(KERN_EMERG "[SSTF] dsp %c %llu\n", direction, blk_rq_pos(closest_rq));
	return 1;
}

/* Essa função adiciona a requisição recebida na fila de requisições. */
static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;

	// add the request to the unordered list (the right request will be picked at dispatch time)
	list_add_tail(&rq->queuelist, &nd->queue);
	#ifdef ENABLE_ACCUM
	add_count++;
	#endif
	printk(KERN_EMERG "[SSTF] add %llu\n", blk_rq_pos(rq));
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e){
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);

	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

/* Infrastrutura dos drivers de IO Scheduling. */
static struct elevator_type elevator_sstf = {
	.ops.sq = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

/* Inicialização do driver. */
static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

/* Finalização do driver. */
static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);

MODULE_AUTHOR("Miguel Xavier");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
