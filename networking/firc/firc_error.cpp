#include "firc.hpp"

firc_error::firc_error(char *error) : error_text(error)
{

}

char *firc_error::get_error()
{
	return error_text;
}
