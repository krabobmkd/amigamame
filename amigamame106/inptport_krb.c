
#include "osdepend.h"
#include "driver.h"
#include "config.h"
#include "xmlfile.h"
#include "inptport.h"
#include "mame.h"
struct _input_port_init_params
{
	input_port_entry *	ports;		/* base of the port array */
	int					max_ports;	/* maximum number of ports we can support */
	int					current_port;/* current port index */
};

// krb 2024, to be called outside of inits.
int driverGetNbPlayers(const struct _game_driver *drv)
{
    if(!drv || !drv->construct_ipt) return 0;

//	// does automalloc:

    struct _input_port_init_params inputs;
    memset(&inputs,0,sizeof(inputs));
    begin_resource_tracking();

    int ires =  input_port_init(drv->construct_ipt);
    int nbplayers=0;
	for(int i=0;i</*inputs.max_ports*/ inputs.current_port;i++)
	{
        int itype = inputs.ports[i].type;
		if(itype >= IPT_START1 && itype <= IPT_START8)
		{
            int nbp = itype-IPT_START1;
            if(nbp>nbplayers) nbplayers=nbp;
		}
	}
    //  drv->construct_ipt(&inputs);
    end_resource_tracking();


	//auto_malloc(1024);
	//auto_malloc_add(NULL); // need that when no tracking
//    if(drv->construct_ipt) drv->construct_ipt(&inputs);
//	int nbplayers=0;
//	for(int i=0;i<inputs.max_ports;i++)
//	{
//		if(inputs.ports[i].player>nbplayers) nbplayers=inputs.ports[i].player ;
//	}
//	// cleanup false init

//	auto_malloc_free();

	return nbplayers+1;

}
