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

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

/*
 * This function dispatches the next block to be read.
 * It assumes that the first element in the queue is the next to be dispatched.
 * The order of the queue must be maintained at the time of adding the request.
 */
static int sstf_dispatch(struct request_queue *q, int force){
	struct sstf_data *nd = q->elevator->elevator_data;
	int head = last_sector_read;
	char direction;
	struct request *rq;
	
	rq = list_first_entry_or_null(&nd->queue, struct request, queuelist);
	if (!rq) return 0;
	list_del_init(&rq->queuelist);
	direction = (head > blk_rq_pos(rq)) ? 'R' : 'L';
	last_sector_read = blk_rq_pos(rq);
	elv_dispatch_sort(q, rq);
	printk(KERN_EMERG "[SSTF] dsp %c %llu\n", direction, blk_rq_pos(rq));
	return 1;
}

/*
 * This function adds the received request to the request queue.
 * The function adds the request to the request queue according to the order established by the SSTF algorithm.
 */
static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;
	int head = last_sector_read;
	struct request *cur_req;
	unsigned long seek_time_cur_req, seek_time_rq = abs(head - blk_rq_pos(rq));

	// add the new request 'rq' immediately before the first request with a greater seek time in the queue
	list_for_each_entry(cur_req, &nd->queue, queuelist) {
		seek_time_cur_req = abs(head - blk_rq_pos(cur_req));
		if (seek_time_cur_req > seek_time_rq) {
			list_add_tail(&rq->queuelist, &cur_req->queuelist); // add 'rq' immediately before 'cur_req' in the queue
			printk(KERN_EMERG "[SSTF] add %llu\n", blk_rq_pos(rq));
			return;
		}
	}

	// if there's no request with a greater seek time than 'rq' in the queue, add it to the end of the queue
	// because it is the greatest seek time
	list_add_tail(&rq->queuelist, &nd->queue);
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