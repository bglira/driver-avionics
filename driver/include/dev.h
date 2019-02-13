/*
 * linux/arinc429/dev.h
 *
 * Definitions for the ARINC429 network device driver interface
 *
 * Copyright (C) 2015 Marek Vasut <marex@denx.de>
 * Updates Copyright (C) 2019 CCX Technologies Inc. <charles@ccxtechnologies.com>
 *
 * Based on the SocketCAN stack.
 */

#ifndef __ARINC429_DEV_H__
#define __ARINC429_DEV_H__

#include "arinc429.h"
#include "netlink.h"

/*
 * ARINC429 mode
 */
enum arinc429_mode {
	ARINC429_MODE_STOP = 0,
	ARINC429_MODE_START,
	ARINC429_MODE_SLEEP
};

/*
 * ARINC429 common private data
 */
struct arinc429_priv {
	struct arinc429_rate rate;
	__u32 ctrlmode;
	__u32 ctrlmode_supported;

	int (*do_set_rate)(struct net_device *dev);
	int (*do_set_mode)(struct net_device *dev, enum arinc429_mode mode);
};

/* Drop a given socketbuffer if it does not contain a valid ARINC429 frame. */
static inline int arinc429_dropped_invalid_skb(struct net_device *dev,
					       struct sk_buff *skb)
{
	if (skb->protocol == htons(ETH_P_ARINC429)) {
		if (unlikely(skb->len % ARINC429_WORD_SIZE))
			goto inval_skb;
	} else
		goto inval_skb;

	return 0;

inval_skb:
	kfree_skb(skb);
	dev->stats.tx_dropped++;
	return 1;
}

struct net_device *alloc_arinc429dev(int sizeof_priv);
void free_arinc429dev(struct net_device *dev);

/* a arinc429dev safe wrapper around netdev_priv */
struct arinc429_priv *safe_arinc429dev_priv(struct net_device *dev);

int open_arinc429dev(struct net_device *dev);
void close_arinc429dev(struct net_device *dev);
int arinc429_change_mtu(struct net_device *dev, int new_mtu);

int register_arinc429dev(struct net_device *dev);
void unregister_arinc429dev(struct net_device *dev);

struct sk_buff *alloc_arinc429_skb(struct net_device *dev,
				   union arinc429_word **cf, int length);

#endif /* __ARINC429_DEV_H__ */