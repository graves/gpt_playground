#!/bin/sh

set -e

DEV=$1
OUTDIR=$2

SEC_GPT_HEAD_BEGIN=$(cgpt show /dev/mmcblk${DEV} | grep "Sec GPT header" | awk '{print $1}')
SEC_PART_TAB_BEGIN=$(cgpt show /dev/mmcblk${DEV} | grep "Sec GPT table" | awk '{print $1}')

echo "Operating on /dev/mmcblk${DEV}"

dd if=/dev/mmcblk${DEV} of=${OUTDIR}/PMBR bs=512 count=1
dd if=/dev/mmcblk${DEV} of=${OUTDIR}/PRIMARY_GPT_HEADER bs=512 count=1 skip=1
dd if=/dev/mmcblk${DEV} of=${OUTDIR}/PRIMARY_PARTITION_TABLE bs=512 count=32 skip=2
dd if=/dev/mmcblk${DEV} of=${OUTDIR}/SECONDARY_GPT_HEADER bs=512 count=1 skip=${SEC_GPT_HEAD_BEGIN}
dd if=/dev/mmcblk${DEV} of=${OUTDIR}/SECONDARY_PARTITION_TABLE bs=512 count=32 skip=${SEC_PART_TAB_BEGIN}
