#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/*
 * 程序清单：这是一个 SPI 设备使用例程
 * 例程导出了 spi_w25q_sample 命令到控制终端
 * 命令调用格式：spi_w25q_sample 
 * 命令解释：命令第二个参数是要使用的SPI设备名称，为空则使用默认的SPI设备
 * 程序功能：通过SPI设备读取 w25q 的 ID 数据
*/

#define W25Q_SPI_DEVICE_NAME          "spi20"   // spi2总线上的0号设备

static void spi_w25q_sample(int argc, char *argv[])
{
  struct rt_spi_device *spi_dev_w25q;
  char name[RT_NAME_MAX];
  rt_uint8_t w25x_read_id = 0x90; // 读设备ID命令
  rt_uint8_t id[5] = {0};

  if (argc == 2)
  {
    rt_strncpy(name, argv[1], RT_NAME_MAX);
  }
  else
  {
    rt_strncpy(name, W25Q_SPI_DEVICE_NAME, RT_NAME_MAX);
  }


  // 查收SPI设备获取设备句柄
  spi_dev_w25q = (struct rt_spi_device *)rt_device_find(name);
  if (!spi_dev_w25q)
  {
    rt_kprintf("spi sample run failed! can't find %s device!\n", name);
  }
  else
  {
    // 方式1：使用rt_spi_send_then_recv()发送命令读取ID
    rt_spi_send_then_recv(spi_dev_w25q, &w25x_read_id, 1, id, sizeof(id));
    rt_kprintf("use rt_spi_send_then_recv() read w25q ID is:%x%x\n", id[3], id[4]); // id[0] - id[2]为0x00

    // 方式2：使用rt_spi_transfer_message()发送命令读取ID
    struct rt_spi_message msg1, msg2;

    msg1.send_buf = &w25x_read_id;
    msg1.recv_buf = RT_NULL;
    msg1.length = 1;
    msg1.cs_take = 1;       // 传输前选中片选
    msg1.cs_release = 0;
    msg1.next = &msg2;

    msg2.send_buf = RT_NULL;
    msg2.recv_buf = id;
    msg2.length = 5;
    msg2.cs_take = 0;
    msg2.cs_release = 1;   // 传输后释放片选
    msg2.next = RT_NULL;

    rt_spi_transfer_message(spi_dev_w25q, &msg1);
    rt_kprintf("use rt_spi_send_then_recv() read w25q ID is:%x%x\n", id[3], id[4]); // id[0] - id[2]为0x00
  }
}

MSH_CMD_EXPORT(spi_w25q_sample, spi w25q sample);

