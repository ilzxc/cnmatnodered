#include "o_expr.h"

using namespace v8;
using namespace std;

o_expr::o_expr(): expression( NULL )
{ }

o_expr::~o_expr()
{
    if ( expression != NULL ) {
        osc_expr_free( expression );
    }
}

void o_expr::Init( Handle< Object > target )
{
    // Prepare constructor template:
    Local< FunctionTemplate > tpl = FunctionTemplate::New( New );
    tpl->SetClassName( String::NewSymbol( "expr" ) );
    tpl->InstanceTemplate()->SetInternalFieldCount( 1 );
    // prototype:
    tpl->PrototypeTemplate()->Set( String::NewSymbol( "set" ),
        FunctionTemplate::New( set )->GetFunction() );
    tpl->PrototypeTemplate()->Set( String::NewSymbol( "eval" ),
        FunctionTemplate::New( eval )->GetFunction() );

    Persistent< Function > constructor = Persistent< Function >::New( tpl->GetFunction() );
    target->Set( String::NewSymbol( "expr" ), constructor );
}

Handle<Value> o_expr::New( const Arguments& args )
{
    HandleScope scope;

    o_expr* obj = new o_expr();
    obj->Wrap( args.This() );
    if ( ! args[ 0 ]->IsUndefined() ) {
        obj->set( args );
    }

    return args.This();
}

Handle<Value> o_expr::set( const Arguments& args )
{
    HandleScope scope;

    if ( args.Length() == 0 ) {
        return scope.Close( Undefined() );
    }

    String::Utf8Value expr_string( args[ 0 ]->ToString() );

    std::string to_parse = std::string( *expr_string );

//    char* to_parse = expr_string;

    o_expr* obj = ObjectWrap::Unwrap< o_expr >( args.This() ); // for accessing our object

    if ( obj->expression != NULL ) {
        osc_expr_free( obj->expression );
        obj->expression = NULL;
    }

    t_osc_err error = osc_expr_parser_parseExpr( to_parse.c_str(), &obj->expression );
    if ( error == OSC_ERR_NONE ) {
        return scope.Close( Undefined() );
    }
    return scope.Close( String::NewSymbol( "ERROR_PARSING_STRING" ) );
}

Handle<Value> o_expr::eval( const Arguments& args )
{
    HandleScope scope;

    o_expr* obj = ObjectWrap::Unwrap< o_expr >( args.This() );

    if ( ( obj->expression == NULL ) || ( args.Length() == 0 ) ) {
        return scope.Close( Undefined() ); // expression not set or no arguments provided
    }

    Local< Object > input = args[ 0 ]->ToObject();
    if ( ! node::Buffer::HasInstance( input ) ) {
        return scope.Close( Undefined() ); // argument is not a Buffer
    }
    char* input_data = node::Buffer::Data( input );
    size_t input_length = node::Buffer::Length( input );

    t_osc_err error = osc_error_bundleSanityCheck( ( int )input_length, input_data );

    if ( error == OSC_ERR_NONE ) {
        char* result = new char[ input_length ];
        long length = input_length;
        memcpy( result, input_data, length );

        t_osc_err error;
        bool no_errors = true;

        t_osc_expr* iterator = obj->expression;
        t_osc_atom_ar_u* out = NULL;

        while ( iterator ) {
            error = osc_expr_eval( iterator, &length, &result, &out );
            if ( error != OSC_ERR_NONE ) {
                no_errors = false;
            }
            iterator = osc_expr_next( iterator );
            osc_atom_array_u_free( out );
            out = NULL;
        }

        node::Buffer* slow_buffer = node::Buffer::New( length );
        memcpy( node::Buffer::Data( slow_buffer ), result, length );
        delete[] result;
        result = NULL;

        // Create Buffer for Output:
        Local< Object > global_obj = Context::GetCurrent()->Global();
        Local< Function > bufferConstructor = Local< Function >::Cast( global_obj->Get( String::New( "Buffer" ) ) );
        Handle< Value > constructor_args[ 3 ] = { slow_buffer->handle_, Integer::New( length ), Integer::New( 0 ) };
        Local< Object > eval_result = bufferConstructor->NewInstance( 3, constructor_args );
        return scope.Close( eval_result );
    }
    return scope.Close( Undefined() );
}
