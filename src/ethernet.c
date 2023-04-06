#include "ethernet.h"
#include "utils.h"
#include "driver.h"
#include "arp.h"
#include "ip.h"
#include <stdio.h>
#include <stdlib.h>
/**
 * @brief 处理一个收到的数据包
 * 
 * @param buf 要处理的数据包
 */
void ethernet_in(buf_t *buf)
{
    // TO-DO
    if(buf->len>14){
        //删除MAC头部
        buf_t buf2;
        buf_copy(&buf2, buf, 0);
        //将data的头部置为0
        memset(buf->data,0,sizeof(ether_hdr_t));
        //修改data指针的位置
        buf_remove_header(buf, sizeof(ether_hdr_t));
        //计算出MAC头部的类型
        ether_hdr_t *hdr = (ether_hdr_t *)buf2.data;
        uint16_t proto=swap16(hdr->protocol16);
        net_in(buf,proto,hdr->src);
        //目的
        printf("\n目的:");
        for (size_t i = 0; i < 6; i++)
        {
            printf("%x ",hdr->dst[i]);
        }
        printf("\n");
        //源
        printf("\n源:");
        for (size_t i = 0; i < 6; i++)
        {
            printf("%x ",hdr->src[i]);
        }
        printf("\n");
    }
}
/**
 * @brief 处理一个要发送的数据包
 * 
 * @param buf 要处理的数据包
 * @param mac 目标MAC地址
 * @param protocol 上层协议
 */
void ethernet_out(buf_t *buf, const uint8_t *mac, net_protocol_t protocol)
{
    // TO-DO
    if(buf->len<46){
        //不足46进行填充
        buf_add_padding(buf,46-buf->len);
    }
    //添加以太网包头,使用该方法通过直接修改hdr达到间接修改buf->data的目的
    buf_add_header(buf, sizeof(ether_hdr_t));
    ether_hdr_t *hdr = (ether_hdr_t *)buf->data;
    //填写目的MAC
    for(int i=0;i<6;i++){
        hdr->dst[i]=mac[i];
    }
    //填写源MAC地址
    for(int i=6;i<12;i++){
        hdr->src[i-6]=mac[i];
    }
    //封装以太网类型,并进行大小端交换
    uint16_t pro=protocol;
    hdr->protocol16=swap16(pro);
    //发送buf
    driver_send(buf);
}
/**
 * @brief 初始化以太网协议
 * 
 */
void ethernet_init()
{
    buf_init(&rxbuf, ETHERNET_MAX_TRANSPORT_UNIT + sizeof(ether_hdr_t));
}

/**
 * @brief 一次以太网轮询
 * 
 */
void ethernet_poll()
{
    if (driver_recv(&rxbuf) > 0)
        ethernet_in(&rxbuf);
}
