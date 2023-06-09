/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd 
 * Copyright (C) 2010 Tobias Brunner
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "android_creds.h"
#include "android_shared_key_credential.h"

#include <plugins/openssl/openssl_rsa_private_key.h>

#include <daemon.h>
#include <threading/rwlock.h>

typedef struct private_android_creds_t private_android_creds_t;

/**
 * Private data of an android_creds_t object
 */
struct private_android_creds_t {

	/**
	 * Public interface
	 */
	android_creds_t public;

	/**
	 * List of trusted certificates, certificate_t*
	 */
	linked_list_t *certs;

	/**
	 * List of trusted certificates, certificate_t*
	 */
        linked_list_t *shared_key_credentials;

	linked_list_t *private_keys;

	/**
	 * read/write lock
	 */
	rwlock_t *lock;

};

/**
 * Certificate enumerator data
 */
typedef struct {
	private_android_creds_t *this;
	key_type_t key;
	identification_t *id;
} cert_data_t;

/**
 * Private key enumerator data
 */
typedef struct {
	private_android_creds_t *this;
	key_type_t key_type;
	identification_t *id;
} private_key_data_t;

/**
 * Filter function for certificates enumerator
 */
static bool cert_filter(cert_data_t *data, certificate_t **in,
						certificate_t **out)
{
	certificate_t *cert = *in;
	public_key_t *public;

	public = cert->get_public_key(cert);
	if (!public)
	{
		return FALSE;
	}
	if (data->key != KEY_ANY && public->get_type(public) != data->key)
	{
		public->destroy(public);
		return FALSE;
	}
	if (data->id && data->id->get_type(data->id) == ID_KEY_ID &&
		public->has_fingerprint(public, data->id->get_encoding(data->id)))
	{
		public->destroy(public);
		*out = cert;
		return TRUE;
	}
	public->destroy(public);
	if (data->id && !cert->has_subject(cert, data->id))
	{
		return FALSE;
	}
	*out = cert;
	return TRUE;
}

/**
 * Filter function for private key enumerator
 */
static bool private_key_filter(private_key_data_t *data, 
			       private_key_t **in, private_key_t **out)
{
  chunk_t chunk;
  private_key_t *private = *in;
  bool match = FALSE;

  if (data->id != NULL &&
      private->get_fingerprint(private, KEYID_PUBKEY_SHA1, &chunk))
    {
      
      identification_t *keyid = identification_create_from_encoding(ID_KEY_ID, chunk);
      match = keyid->equals(keyid, data->id);
      keyid->destroy(keyid);

      if (match)
	{
	  *out = *in;
	}    
    }

  DBG2(DBG_CFG, "android_creds_t:  private_key_filter: %s", (match)?("MATCH"):("NO MATCH"));
  return match;
}

/**
 * Destroy certificate enumerator data
 */
static void private_key_data_destroy(private_key_data_t *this)
{
	this->this->lock->unlock(this->this->lock);
	free(this);
}

/**
 * Destroy certificate enumerator data
 */
static void cert_data_destroy(cert_data_t *this)
{
	this->this->lock->unlock(this->this->lock);
	free(this);
}

METHOD(credential_set_t, create_cert_enumerator, enumerator_t*,
	   private_android_creds_t *this, certificate_type_t cert, key_type_t key,
	   identification_t *id, bool trusted)
{
	if (cert == CERT_X509 || cert == CERT_ANY)
	{
		cert_data_t *data;
		this->lock->read_lock(this->lock);
		INIT(data, .this = this, .id = id, .key = key);
		return enumerator_create_filter(
						this->certs->create_enumerator(this->certs),
						(void*)cert_filter, data, (void*)cert_data_destroy);
	}
	return NULL;
}

METHOD(credential_set_t, create_private_enumerator, enumerator_t*,
	   private_android_creds_t *this, key_type_t type, identification_t *id)
{
  private_key_data_t *data;

  this->lock->read_lock(this->lock);
  INIT(data, .this = this, .id = id, .key_type = type);
  return enumerator_create_filter(
				  this->private_keys->create_enumerator(this->private_keys),
				  (void*)private_key_filter, data, (void*)private_key_data_destroy);
}


/**
 * Shared key enumerator implementation
 */
typedef struct {
	enumerator_t public;
	private_android_creds_t *this;
	shared_key_t *key;
	bool done;
} shared_enumerator_t;

METHOD(enumerator_t, shared_enumerate, bool,
	   shared_enumerator_t *this, shared_key_t **key, id_match_t *me,
	   id_match_t *other)
{
	if (this->done)
	{
		return FALSE;
	}
	*key = this->key;
	if (me != NULL)
	  {
	    *me = ID_MATCH_PERFECT;
	  }
	
	if (other != NULL)
	  {
	    *other = ID_MATCH_ANY;
	  }
	this->done = TRUE;
	return TRUE;
}

METHOD(enumerator_t, shared_destroy, void,
	   shared_enumerator_t *this)
{
	this->key->destroy(this->key);
	this->this->lock->unlock(this->this->lock);
	free(this);
}

METHOD(credential_set_t, create_shared_enumerator, enumerator_t*,
	   private_android_creds_t *this, shared_key_type_t type,
	   identification_t *me, identification_t *other)
{
  shared_enumerator_t *enumerator = NULL;
  enumerator_t *list_enumerator = NULL;
  shared_key_credential_t* credential = NULL;

  this->lock->read_lock(this->lock);
  
  if (this->shared_key_credentials->get_count(this->shared_key_credentials) == 0)
    {
      this->lock->unlock(this->lock);
      return NULL;
    }

  list_enumerator = 
    this->shared_key_credentials->create_enumerator(this->shared_key_credentials);  

  while(me != NULL && list_enumerator->enumerate(list_enumerator, &credential))
    {
      if (me && me->equals(me, credential->id) && type == credential->key_type)
        {
          break;
        }
      else
        {
          credential = NULL;
        }
    }
  list_enumerator->destroy(list_enumerator);

  if (credential == NULL)
    {
      this->lock->unlock(this->lock);
      return NULL;
    }
 
  INIT(enumerator,
       .public = {
         .enumerate = (void*)_shared_enumerate,
           .destroy = _shared_destroy,
           },
       .this = this,
       .done = FALSE,
       .key = credential->key->get_ref(credential->key),
       );
  

  return &enumerator->public;
}

METHOD(android_creds_t, add_certificate, void,
	   private_android_creds_t *this, char *cert_pem)
{
	certificate_t *cert = NULL;
	chunk_t chunk;

	chunk.ptr = cert_pem;
	chunk.len = strlen(cert_pem);

	cert = lib->creds->create(lib->creds, CRED_CERTIFICATE, CERT_X509,
				  BUILD_BLOB_PEM, chunk, BUILD_END);
	if (cert)
	  {
	    this->lock->write_lock(this->lock);
	    this->certs->insert_last(this->certs, cert);
	    this->lock->unlock(this->lock);
	  }
	else
	  {
	    DBG1(DBG_CFG, "Failed to create certificate.");
	  }
}

METHOD(android_creds_t, add_private_key, bool,
	   private_android_creds_t *this, char *name)
{
	bool status = FALSE;

	openssl_rsa_private_key_t *key = 
	  openssl_rsa_private_key_create("keystore", name);	
	
	if (key == NULL)
	  {
	    DBG1(DBG_CFG, "Failed to create key");
	    return status;
	  }

	this->lock->write_lock(this->lock);
	this->private_keys->insert_last(this->private_keys, key);
	this->lock->unlock(this->lock);

	status = TRUE;
	return status;
}

METHOD(android_creds_t, set_username_password, void,
       private_android_creds_t *this, identification_t *id, char *password,
       bool is_xauth)
{
	identification_t *new_id = NULL;
	shared_key_type_t key_type = is_xauth ? SHARED_EAP : SHARED_IKE;
	shared_key_t* key = NULL;
	shared_key_credential_t* shared_key_credential = NULL;

	new_id = id->clone(id);
	key = shared_key_create(SHARED_IKE, chunk_clone(chunk_create(password,
								     strlen(password))));
	shared_key_credential = shared_key_credential_t_create(new_id, key, key_type);

	this->lock->write_lock(this->lock);
	this->shared_key_credentials->insert_first(this->shared_key_credentials, 
						   shared_key_credential);  
	this->lock->unlock(this->lock);	
}

METHOD(android_creds_t, clear, void,
	   private_android_creds_t *this)
{
	certificate_t *cert = NULL;
	private_key_t *private_key = NULL;
	shared_key_credential_t* shared_key = NULL;

	this->lock->write_lock(this->lock);
	while (this->certs->remove_last(this->certs, (void**)&cert) == SUCCESS)
	{
		cert->destroy(cert);
		cert = NULL;
	}

	while( this->shared_key_credentials->remove_first(this->shared_key_credentials, 
							  (void**)&shared_key) == SUCCESS)
        {
        	shared_key->destroy(shared_key);
        	shared_key = NULL;
        }

	while (this->private_keys->remove_last(this->private_keys, (void**)&private_key) == SUCCESS)
	  {
	    private_key->destroy(private_key);
	  }

	this->lock->unlock(this->lock);
}

METHOD(android_creds_t, destroy, void,
	   private_android_creds_t *this)
{
	clear(this);
	this->certs->destroy(this->certs);
	this->private_keys->destroy(this->private_keys);
	this->shared_key_credentials->destroy(this->shared_key_credentials);
	this->lock->destroy(this->lock);
	free(this);
}

/**
 * Described in header.
 */
android_creds_t *android_creds_create()
{
	private_android_creds_t *this;

	INIT(this,
		.public = {
			.set = {
				.create_cert_enumerator = _create_cert_enumerator,
				.create_shared_enumerator = _create_shared_enumerator,
				.create_private_enumerator = _create_private_enumerator,
				.create_cdp_enumerator = (void*)return_null,
				.cache_cert = (void*)nop,
			},
			.add_certificate = _add_certificate,
			.add_private_key = _add_private_key,
			.set_username_password = _set_username_password,
			.clear = _clear,
			.destroy = _destroy,
		},
		.certs = linked_list_create(),
		.private_keys = linked_list_create(),
		.shared_key_credentials = linked_list_create(),
		.lock = rwlock_create(RWLOCK_TYPE_DEFAULT),
	);

	return &this->public;
}

