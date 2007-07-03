# $Header: /data/zender/nco_20150216/nco/data/swamp.sh,v 1.4 2007-07-03 02:51:35 zender Exp $

# Purpose: Demonstrate SWAMP usage

# Usage:
# Using SWAMP requires two steps
# First, identify the SWAMP server with access to your data, e.g.:
# export SWAMPURL='http://sand.ess.uci.edu:8081/SOAP'
# export SWAMPURL='http://pbs.ess.uci.edu:8081/SOAP'
# Second, call your script as an argument to SWAMP's invocation:
# python ~/nco/src/ssdap/swamp_client.py ~/nco/data/swamp.sh
# Whitespace-separated list of directories in sand:${DATA}/swamp_include is exported
# to PBS (pbs.ess.uci.edu) for processing by SWAMP server there.

export caseid='cssnc2050_02b'

flg_tst='1' # [flg] Test mode
flg_prd='0' # [flg] Production mode

flg_typ=${flg_tst} # [enm] Run type

if [ "${flg_typ}" = "${flg_tst}" ] ; then
    ncra -O ~/nco/data/in.nc ~/foo.nc
elif [ "${flg_typ}" = "${flg_prd}" ] ; then 
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-01.nc ~/${caseid}_clm01.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-02.nc ~/${caseid}_clm02.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-03.nc ~/${caseid}_clm03.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-04.nc ~/${caseid}_clm04.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-05.nc ~/${caseid}_clm05.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-06.nc ~/${caseid}_clm06.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-07.nc ~/${caseid}_clm07.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-08.nc ~/${caseid}_clm08.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-09.nc ~/${caseid}_clm09.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-10.nc ~/${caseid}_clm10.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-11.nc ~/${caseid}_clm11.nc
    ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-12.nc ~/${caseid}_clm12.nc
    ncra -O ~/${caseid}_clm??.nc ~/${caseid}_clm.nc
fi # !prd


